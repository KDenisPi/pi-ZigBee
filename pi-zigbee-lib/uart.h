/**
 * uart.h
 *
 * UART support
 *
 *  Created on: Apr 1, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_UART_H_
#define PI_ZIGBEE_LIB_UART_H_

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <memory>
#include <string>
#include <poll.h>
#include <sys/select.h>
#include <functional>

#include "Threaded.h"
#include "logger.h"
#include "CircularBuffer.h"
#include "uart_frame.h"
#include "uart_efr_buff.h"
#include "ezsp_util.h"

namespace zb_uart {

using EFramePtr = std::shared_ptr<EFData>;
using EframeBuff = piutils::circbuff::CircularBuffer<EFramePtr>;

/**
 * UART transmissionstructure
 */
class ZBUart_Info {
public:
    friend ZBUart;

    ZBUart_Info() : _frmNum(7), _ackNum(0), _fd(-1) {}
    ~ZBUart_Info() {}

    /**
     * 5.2 Acknowledgements and Frame Numbers
    The ackNum field in DATA, ACK, and NAK frames acknowledges received DATA frames. Note that ackNum is the number of the next
    frame the receiver expects and it is one greater than the last frame received.
    The NCP discards any frames it receives that have an invalid ackNum value. A valid ackNum is a number between the last received
    ackNum and the last transmitted frmNum plus one, where both limits are inclusive.
    The maximum number of frames a sender can transmit without them being acknowledged is the window size, which is specified by the
    parameter TX_K. When a sender has TX_K unacknowledged frames, it may not send any more, although it may retransmit frames if
    needed.
    */
    uint8_t get_next_frmNum(){
        _frmNum++;
        if(_frmNum>7) _frmNum = 0;

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Current: " + std::to_string(_frmNum));
        return _frmNum;
    }

    uint8_t get_current_frmNum() const {
        return _frmNum;
    }

    void set_ackNum(const uint8_t ackNum){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Current: " + std::to_string(_ackNum) + " Received: " + std::to_string(ackNum));

        if(ackNum>7)
            _ackNum = 0;
        else
            _ackNum = ackNum;
    }

    uint8_t get_current_ackNum() const {
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Current: " + std::to_string(_ackNum));
        return _ackNum;
    }

    /**
     * TODO:
     */
    uint8_t get_ackNum(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Next: " + std::to_string(_ackNum));
        return _ackNum;
    }

    /**
     * Device initialized and full ready to work
     */

    const bool is_connected() const {
        return _connected;
    }

    void set_connected(const bool connected){
        _connected = connected;
    }

    /**
     * Set Access to device
     */
    const bool is_device_connected() const{
        return (_fd > 0);
    }

    /**
     * Logical start/stop
     */
    const bool is_activated() const {
        return _activated;
    }

    void set_activate(const bool activate) {
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Activated: " + std::to_string(activate));
        _activated = activate;
    }

protected:
    uint8_t _frmNum; //currect frmNum
    uint8_t _ackNum; //acknowledges receipt of DATA frames up to, but not including, ackNum

    int _fd; //connection FD

    /**
     * CONNECTED State
     * The Host initializes the ASH protocol by resetting the NCP through either the nRESET pin or sending the RST frame. When the NCP is
     * fully booted and ready to interact with the Host, the NCP will send the RSTACK frame to the Host.
     */
    bool _connected;    //Is connected state

    volatile bool _activated;    //UART is ready for starting process input requests

    int _stopBits = 1;
    bool _rts_cts_flow_ctrl = false;
    bool _ixon = true;
    bool _ixoff = true;
    uint8_t _read_min = 0;
    uint8_t _read_timeout = 10;  //10 tenths of a second - 1 sec
};

/**
 *
 */

class Ezsp;
class ZBUart : public piutils::Threaded {
public:

    friend Ezsp;
    /**
     *
     */
    ZBUart(const bool debug_mode = false) : _debug(debug_mode) {
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        _eframes = std::make_shared<EframeBuff>(20);
    }

    /**
     *
     */
    ~ZBUart(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        //Stop working thread
        stop();

        //close device connection
        disconnect();
    }

    inline void set_activate(const bool activate) {
        std::lock_guard<std::mutex> lk(cv_m);
        _info->set_activate(activate);
        cv.notify_one();
    }

    /**
     * Logical start/stop
     */
    const bool is_activated() const {
        return _info->is_activated();
    }

    /**
     *
     */
    bool start(){
        return piutils::Threaded::start<ZBUart>(this);
    }


    const std::vector<uint8_t> spec = {ashrvd::FlagByte, ashrvd::EscapeByte, ashrvd::XON, ashrvd::XOFF, ashrvd::SubstByte, ashrvd::CancelByte};

    /**
     *
     */
    bool connect(const std::string& port, const int bitrate){
        bool result = false;

        if(_info->_fd>0){
            return true;
        }

        _info->_fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if(_info->_fd < 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not open device: " + std::to_string(errno));
            return false;
        }


        /**
         * Configure speed
         */
        struct termios tio;
        int res = tcgetattr(_info->_fd, &tio);
        if(res < 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not get device attributes: " + std::to_string(errno));
        }
        else{
            if (cfsetospeed(&tio, B57600)<0 || cfsetispeed(&tio, B57600)<0){
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set speed: " + std::to_string(errno));
            }
            else{
                tio.c_cflag |= CLOCAL;               // ignore modem inputs
                tio.c_cflag |= CREAD;                // receive enable (a legacy flag)
                tio.c_cflag &= ~CSIZE;               // 8 data bits
                tio.c_cflag |= CS8;
                tio.c_cflag &= ~PARENB;              // no parity

                if (_info->_stopBits == 1) { //0 - 1-bit
                    tio.c_cflag &= ~CSTOPB;
                } else {
                    tio.c_cflag |= CSTOPB; //1 - 2-bits
                }

                //Enable RTS/CTS (hardware) flow control
                if (_info->_rts_cts_flow_ctrl) {
                    tio.c_cflag |= CRTSCTS;
                } else {
                    tio.c_cflag &= ~CRTSCTS;
                }

                tio.c_iflag &= ~( BRKINT | INLCR | IGNCR | ICRNL | INPCK |
                                            ISTRIP | IMAXBEL | IXON | IXOFF | IXANY );

                //Enable start/stop output flow control
                if (_info->_ixon) {
                    tio.c_iflag |= IXON;
                } else {
                    tio.c_iflag &= ~IXON;
                }

                //Enable start/stop input flow control
                if (_info->_ixoff) {
                    tio.c_iflag |= IXOFF;          // SW flow control
                } else {
                    tio.c_iflag &= ~IXOFF;
                }

                tio.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG ); // raw input
                tio.c_oflag &= ~OPOST;               // raw output

                memset(tio.c_cc, _POSIX_VDISABLE, NCCS);  // disable all control chars
                tio.c_cc[VSTART] = CSTART;           // define XON and XOFF
                tio.c_cc[VSTOP] = CSTOP;

                /**
                 * The TIME element (indexed using the constant VTIME ) specifies a timeout value in tenths of a second.
                 * The MIN element (indexed using VMIN ) specifies the minimum number of bytes to be read.
                 *
                 * MIN == 0, TIME == 0 (polling read)
                 * MIN > 0, TIME == 0 (blocking read)
                 * MIN == 0, TIME > 0 (read with timeout)
                 * MIN > 0, TIME > 0 (read with interbyte timeout)
                 */
                tio.c_cc[VMIN] = _info->_read_min;
                tio.c_cc[VTIME] = _info->_read_timeout;

                if (tcsetattr(_info->_fd, TCSAFLUSH, &tio)<0){
                    logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set attribute: " + std::to_string(errno));
                }
                else{
                    result = true;
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART configured");

                    get_info();
                }
            }
        }

       if(!result){
           disconnect();
       }

        return result;
    }

    /**
     *
     */
    void disconnect(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        if(_info->_fd >= 0){
            int res = close(_info->_fd);
            _info->_fd = -1;
        }
    }

    /**
     *
     * Copy and move is not allowed
    */
    ZBUart(const ZBUart&) = delete;
    ZBUart(ZBUart&&) = delete;
    ZBUart& operator=(const ZBUart&) = delete;
    ZBUart& operator=(ZBUart&&) = delete;

    /**
     * Compose DATA type frame
     */
    std::shared_ptr<UFrame> compose_data(const uint8_t* data, const size_t len){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        std::shared_ptr<UFrame> frame = std::shared_ptr<UFrame>(new UFrame(ftype::DATA));
        //copy data
        frame->set_data(data, len);

        return frame;
    }

    /**
     * Compose ACK, NAK, RST type frame
     */
    std::shared_ptr<UFrame> compose(const ftype ft, const bool nRdy = false){
        assert(ft!=ftype::DATA);
        assert(ft!=ftype::RSTACK);
        assert(ft!=ftype::ERROR);

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Type: " + std::to_string(ft));

        std::shared_ptr<UFrame> frame = std::shared_ptr<UFrame>(new UFrame(ft));
        return frame;
    }

    /**
     * Parse received data.
     * If data is incorrect then return emty object
     */
    std::shared_ptr<UFrame> parse(const uint8_t* buffer, const size_t len, const bool randm = false){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Reived buffer with length: " + std::to_string(len));

        bool result = true;
        std::shared_ptr<UFrame> frame = std::shared_ptr<UFrame>(new UFrame());

        if(_debug){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " " + zb_ezsp::Conv::print_buff(buffer, len));
        }

        uint8_t tmp_buff[140];
        size_t r_len = decode(buffer, len, tmp_buff, sizeof(tmp_buff));

        if(_debug){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " " + zb_ezsp::Conv::print_buff(tmp_buff, r_len));
        }

        //incorrect length or error data decoding
        if(r_len<4 || r_len>133){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length or decoding error");
            frame.reset();
            return frame;
        }

        uint8_t last_byte = tmp_buff[r_len-1];

        frame->set_control_byte(tmp_buff[0]);   //save received control byte
        frame->set_data(&tmp_buff[1], r_len-4); //save received data

        uCRC crc;
        crc.bytes[1] = tmp_buff[r_len-3];
        crc.bytes[0] = tmp_buff[r_len-2];
        frame->crc(crc.crc);                    //save CRC

        //check last byte
        //Generally speaking on case CancelByte and SubstByte - data should be discarded
        if(ashrvd::CancelByte == last_byte || ashrvd::FlagByte == last_byte || ashrvd::SubstByte){
            frame->set_flag_byte(last_byte);
        }
        else{
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect Flag Byte");
            frame.reset();
            return frame;
        }

        //incorrect type - RST can be send from Host only (ignore this check on debug mode)
        if(frame->is_RST() && !_debug){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame type (RST)");
            result = false;
        }
        else if((frame->is_RSTACK() || frame->is_ERROR()) && r_len != 6){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length (RSTACK, ERROR)");
            result = false;
        }
        else if(frame->is_DATA() && r_len < 7){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length (DATA)");
            result = false;
        }
        else if((frame->is_ACK() || frame->is_NAK()) && r_len != 4){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length (ACK, NAK)");
            result = false;
        }

        //validate CRC
        if(frame->is_DATA() || frame->is_ACK() || frame->is_NAK() || _debug){
            uint16_t crc = CRC16::crc_ccitt(tmp_buff, r_len-3);
            uint16_t crc_recv = frame->get_crc();

            if(crc != crc_recv){
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " CRC Error");
                result = false;
            }
        }

        //Un-randomize
        if(randm && frame->data_len() > 0){
            randomize(frame->raw_data(), frame->data_len());
        }

        /**
         * Have a valid ackNum if in the CONNECTED state and the frame type is DATA, ACK, or NAK.
         */
        //TODO

        if(!result){
            frame.reset();
        }
        else{
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Frame info:" + frame->to_string());
        }

        return frame;
    }

    /**
     * Decode frame buffer after receiving
     */
    size_t decode(const uint8_t* in_buff, const size_t len, uint8_t* out_buff, const size_t out_len) {
        assert(len>=4);

        size_t j = 0;
        for(int i=0; i<len && j<out_len; i++){
            if(UFrame::BEscape == in_buff[i]){
                out_buff[j++] = stuffing(in_buff[++i]);
            }
            else
            {
                out_buff[j++] = in_buff[i];
            }
        }

        return (j==out_len ? 0 : j);
    }

    /**
     * Transcode buffer before sending
     *
     * 4.1 Sending Frames
        Before ASH sends a DATA frame, it performs this series of steps:
        1. The Control Byte is added before the Data Field. The frmNum field is set to the last frame transmitted plus one, and the ackNum
            field is set to the number of the next frame expected to be received. The reTx flag is clear.
        2. The Data Field is exclusive-OR’ed with a pseudo-random sequence (see Data Randomization).
        3. The two-byte CRC of the Control Byte plus the Data Field is computed and appended after the Data Field.
        4. The frame is byte stuffed to remove reserved byte values (see Reserved Bytes and Byte Stuffing).
        5. A Flag Byte is added after the CRC.
            If a DATA frame is retransmitted, the process is the same except for step 1. The frmNum field retains the same value as when the
            frame was first transmitted, and the reTx bit is set. The ackNum is the current value as in normal transmission.
            Other frame types omit step 2 and have differently formatted Control Bytes, but otherwise they use the same process.


            Note: Skip Stuffing used for debug purposes only
     */
    size_t encode(const std::shared_ptr<UFrame> frame, uint8_t* out_buff, const size_t len, const bool randm = false, const bool skip_stuffing = false) const {
        size_t out_len = 0, i = 0;
        uint8_t tmp_buff[133];
        memset(tmp_buff, 0x00, sizeof(tmp_buff));

        /*
         4.1.0 for DATA
        */
       if(frame->is_DATA()){
           frame->set_frmNum(_info->get_next_frmNum());
           frame->set_ackNum(_info->get_ackNum());
       }
       else if(frame->is_ACK() || frame->is_NAK()){
           frame->set_ackNum(_info->get_ackNum());
       }

        //4.1.1 Set Control byte
        tmp_buff[0] = frame->control_byte();
        out_len++;

        //4.1.2 Randomize data
        if(frame->data_len() > 0){
            memcpy(&tmp_buff[1], frame->data(), frame->data_len());

            if(randm)
                randomize(&tmp_buff[1], frame->data_len());

            out_len += frame->data_len();

        }

        //4.1.3
        uCRC crc;
        crc.crc = CRC16::crc_ccitt(tmp_buff, out_len);
        tmp_buff[out_len++] = crc.bytes[1];
        tmp_buff[out_len++] = crc.bytes[0];

        //

        //4.1.4 Byte Stuffing  TODO: Improve buffer size check
        if(_debug && skip_stuffing){
            memcpy(out_buff, tmp_buff, out_len);
        }
        else{
            int out_len_mod = 0;
            for(i=0; i<out_len &&out_len_mod<(len-2); i++){
                for(const auto vl : spec){
                    if(vl == tmp_buff[i]){
                        out_buff[out_len_mod++] = UFrame::BEscape;
                        out_buff[out_len_mod++] = stuffing(vl);
                        break;
                    }
                    if(ashrvd::CancelByte== vl){
                        out_buff[out_len_mod++] = tmp_buff[i];
                    }
                }
            }

            out_len = out_len_mod;
        }


        //4.1.5 Flag Byte
        out_buff[out_len++] = ashrvd::FlagByte;
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Frame prepared for sending Len: " + std::to_string(out_len));

        return out_len;
    }


    /**
     * 4.3 Data Randomization
     *  If a DATA frame contains many reserved bytes, byte stuffing can nearly double the length of the frame. To make this less likely, a DA-
     *  TA frame’s Data Field is exclusive-OR’ed with the output of a pseudo-random sequence before byte stuffing. The Data Field is restored
     *  by exclusive-OR’ing again at the receiver.
     */
    static const uint8_t Rand0 = 0x42;

    static void randomize(uint8_t* buff, const size_t len){
        uint8_t rand = Rand0;
        for(int i = 0; i < len; i++){
            uint8_t b = buff[i] ^ rand;
            buff[i] = b;
            if((rand%2)==0)
                rand = (rand >>1);
            else
            {
                rand = (rand >>1) ^ 0xB8;
            }
        }
    }

    /**
     * 4.2 Reserved Bytes and Byte Stuffing
     *  ASH reserves certain byte values for special functions. If bytes with these values happen to occur within a frame, ASH uses a process
     *   known as byte stuffing to replace those bytes so they have non-reserved values. Byte stuffing is performed on the entire ASH frame
     *   except for the Flag Byte. The receiver reverses the process to recover the original frame contents.
     */
    constexpr uint8_t stuffing(uint8_t rv) const {
        return (rv&0x20 ? (rv&0xDF) : (rv|0x20));
    }


    static void worker(ZBUart* p_uart);

    /**
     * Initialize device
     */
    bool init_device(const int attempts = 3) {
        uint8_t wr_buff[255], rd_buff[255];
        int fdnm, res;
        bool result = false;
        int attempt_cnt = 0;

        if(!_info->is_device_connected()){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " No connection to device");
            return false;
        }

        //create RST frame
        std::shared_ptr<UFrame> fr_rst = compose(ftype::RST);
        size_t wr_len = encode(fr_rst, wr_buff, sizeof(wr_buff), true);

        if(_debug){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " RST: " + zb_ezsp::Conv::print_buff(wr_buff, wr_len));
        }

        res = write_data(wr_buff, wr_len);
        if(res != 0){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Write failed");
            return result;
        }

        while(1){
            attempt_cnt++;
            if(attempt_cnt>attempts){
                break;
            }

            size_t b_len = sizeof(rd_buff);
            res = read_data(rd_buff, b_len);
            if(res!=0){ //error
                if(res == ETIME){ //Timeout - resend RST frame
                    if(_debug){
                        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Timeout ");
                    }

                    //resend RST
                    res = write_data(wr_buff, wr_len);
                    if(res != 0){
                        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Write failed");
                        break;
                    }

                    continue; //try to read respose again
                }

                break; //if any other error - break processing
            }
            else{
                //read some data finished by FlagByte or other end frame byte (Ignore this data)
                if(b_len==0){
                    continue;
                }

                if(_debug){
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " RSTACK: " + zb_ezsp::Conv::print_buff(rd_buff, b_len));
                }

                std::shared_ptr<UFrame> fr_rsv = parse(rd_buff, b_len);
                if(fr_rsv){
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Received: " + fr_rsv->to_string());

                    if(fr_rsv->is_RSTACK()){ //success
                        result = true;
                        break;
                    }
                }
                //we read something but there is not RSTACK - try again
            }
        }

        return result;
    }

    /**
     * Write data buffer to device
     */
    int write_data(uint8_t* buff, size_t len){
        int res = write(_info->_fd, buff, len);
        if(res == -1){
            int err = errno;
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Error: " + std::to_string(err));
            if(err == EBADF){
                //close connection and try reconnect again
                disconnect();
            }

            return err;
        }
        else{
            if(res < len){
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Wrote less Len: " + std::to_string(len) + " Real: " + std::to_string(res));
            }
        }

        if(_debug){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Wrote bytes: " + std::to_string(len) + " Res: " + std::to_string(res));
        }

        return 0;
    }

    /**
     * Read data from device
     */
    int read_data(uint8_t* buffer, size_t& len){
        uint8_t r_buff[1];
        volatile size_t read_bytes = 0;

        memset(buffer, 0x00, len);

        while(read_bytes<len){

            int res = read(_info->_fd, r_buff, 1);
            if(res == -1){
                int err = errno;
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Read error: " + std::to_string(err));
                if(err == EBADF){
                    //close connection and try reconnect again
                    disconnect();
                }
                return err;
            }

            if(res==0){ //timeout
                len = 0;
                if(_debug){
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) +  " Timeout! Read bytes: " + std::to_string(read_bytes) + " "  + zb_ezsp::Conv::print_buff(buffer, read_bytes));
                    //std::cout << "Byte " << read_bytes << " " << zb_ezsp::Conv::print_buff(buffer, len) << std::endl;
                }
                return 0;
            }

            //check do we read end of frame already
            if(res > 0){
                //std::cout << "Byte " << read_bytes << " Res: " << res << " " << std::hex << (uint16_t)r_buff[0] << std::endl;

                //if we received frame finished by Cancel or Subst byte - ignore it
                if(ashrvd::CancelByte == r_buff[0] || ashrvd::SubstByte == r_buff[0]){
                    logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Cancel or Subst detected at Pos: " + std::to_string(read_bytes));
                    //TODO: process not the end of chunk
                    read_bytes = 0;
                }
                else if(ashrvd::FlagByte == r_buff[0]){ //frame end
                    buffer[read_bytes] = r_buff[0];
                    read_bytes++;
                    len = read_bytes;
                    if(_debug){
                        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) +  " Read bytes: " + std::to_string(len) + " "  + zb_ezsp::Conv::print_buff(buffer, len));
                        //std::cout << "Byte " << read_bytes << " " << zb_ezsp::Conv::print_buff(buffer, len) << std::endl;
                    }

                    return 0;
                }
                else{
                    //save received byte
                    buffer[read_bytes] = r_buff[0];
                    read_bytes++;
                }
            }
        }

        return 0;
    }


    /**
     * Send UART Frame
     */
    int send_frame(const std::shared_ptr<zb_uart::UFrame>& fr){
        uint8_t w_buff[140];
        memset(w_buff, 0x00, sizeof(w_buff));

        size_t wr_len = encode(fr, w_buff, sizeof(w_buff), true);

        if(is_debug()){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART Frame: " + fr->to_string());
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART Frame : " + zb_ezsp::Conv::print_buff(w_buff, wr_len));
        }

        int wr_res = write_data(w_buff, wr_len);
        return wr_res;
    }

    /**
     * Add EZSP frame to sending queue
     */
    void put_ezsp_frame(EFramePtr& eframe){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART Frame: Add frame to queue");
        _eframes->put(eframe);
    }

    const EFramePtr get_ezsp_frame(){
        if(_eframes->is_empty()){
            const EFramePtr eptr = EFramePtr();
            return eptr;
         }

         return _eframes->get();
    }


    /**
     * Callbacks
     *
     * callback_connected - Called when UART init_device successfully finished
     * callback_eframe_received - Called when UART received DATA frame
     *
     */

    std::function<void(const bool)> callback_connected;
    std::function<void(const EFramePtr&)> callback_eframe_received;


private:

    std::shared_ptr<EframeBuff> _eframes;

    /**
     * Print terminal configurationinformation
     */
    void get_info() const {
        struct termios tio;

        int res = tcgetattr(_info->_fd, &tio);
        if(res != 0){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not get device attributes: " + std::to_string(errno));
            return;
        }

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Input flags: " + std::to_string(tio.c_iflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNBRK  " + std::to_string(tio.c_iflag&IGNBRK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " BRKINT  " + std::to_string(tio.c_iflag&BRKINT));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNPAR  " + std::to_string(tio.c_iflag&IGNPAR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " PARMRK  " + std::to_string(tio.c_iflag&PARMRK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " INPCK   " + std::to_string(tio.c_iflag&INPCK));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ISTRIP  " + std::to_string(tio.c_iflag&ISTRIP));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " INLCR   " + std::to_string(tio.c_iflag&INLCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IGNCR   " + std::to_string(tio.c_iflag&IGNCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ICRNL   " + std::to_string(tio.c_iflag&ICRNL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXON    " + std::to_string(tio.c_iflag&IXON));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXANY   " + std::to_string(tio.c_iflag&IXANY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IXOFF   " + std::to_string(tio.c_iflag&IXOFF));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IMAXBEL " + std::to_string(tio.c_iflag&IMAXBEL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " IUTF8   " + std::to_string(tio.c_iflag&IUTF8));

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Output flags: " + std::to_string(tio.c_oflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OPOST  " + std::to_string(tio.c_oflag&OPOST));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OLCUC  " + std::to_string(tio.c_oflag&OLCUC));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OLCUC  " + std::to_string(tio.c_oflag&OLCUC));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OCRNL  " + std::to_string(tio.c_oflag&OCRNL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ONOCR  " + std::to_string(tio.c_oflag&ONOCR));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " ONLRET " + std::to_string(tio.c_oflag&ONLRET));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OFILL  " + std::to_string(tio.c_oflag&OFILL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " OFDEL  " + std::to_string(tio.c_oflag&OFDEL));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " NLDLY  " + std::to_string(tio.c_oflag&NLDLY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " CRDLY  " + std::to_string(tio.c_oflag&CRDLY));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " TABDLY " + std::to_string(tio.c_oflag&TABDLY));

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Control flags: " + std::to_string(tio.c_cflag));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " CRTSCTS " + std::to_string(tio.c_cflag&CRTSCTS));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " CSIZE   " + std::to_string(tio.c_cflag&CSIZE));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " CSTOPB  " + std::to_string(tio.c_cflag&CSTOPB));
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " PARENB  " + std::to_string(tio.c_cflag&PARENB));


        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Local flags: " + std::to_string(tio.c_lflag));

        //Print speed
        speed_t irate, orate;
        irate = cfgetispeed(&tio);
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Input speed: " + std::to_string(irate));
        orate = cfgetospeed(&tio);
        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Output speed:" + std::to_string(orate));
    }

    /**
     * Get session information
     */
    std::shared_ptr<ZBUart_Info> get_session_info() {
        return _info;
    }

    const bool is_debug() const {
        return _debug;
    }

private:
    std::shared_ptr<ZBUart_Info> _info = std::make_shared<ZBUart_Info>(); //session information

    bool _debug;
};

} //end namespace zb_uart

#endif
