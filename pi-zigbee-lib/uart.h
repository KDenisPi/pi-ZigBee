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

#include "Threaded.h"
#include "logger.h"
#include "uart_frame.h"

namespace zb_uart {

/**
 * UART transmissionstructure
 */
class ZBUart_Info {
public:
    friend ZBUart;

    ZBUart_Info() : _frmNum(7), _ackNum(0), _fd(-1) {}
    ~ZBUart_Info() {}

    uint8_t frmNum(){
        _frmNum++;
        if(_frmNum>7) _frmNum = 0;

        return _frmNum;
    }

    void set_ackNum(const uint8_t ackNum){
        _ackNum = ackNum;
    }

    uint8_t ackNum() const {
        return _ackNum;
    }

    const bool is_connected() const {
        return _connected;
    }

    void set_connected(const bool connected){
        _connected = connected;
    }

    const bool is_started() const{
        return (_fd > 0);
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

};

/**
 *
 */
class ZBUart : public piutils::Threaded {
public:
    /**
     *
     */
    ZBUart(const bool debug_mode = false) : _debug(debug_mode) {
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));
    }

    /**
     *
     */
    ~ZBUart(){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__));

        disconnect();
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
            if (cfsetospeed(&tio, B57600)  < 0){
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set speed: " + std::to_string(errno));
            }
            else{
                if (tcsetattr(_info->_fd, TCSAFLUSH, &tio) < 0){
                    logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not set attribute: " + std::to_string(errno));
                }
                else{
                    result = true;
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART configured");
                }
            }
        }

       if(!result){
           disconnect();
       }
       else
           get_info();

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
    std::shared_ptr<UFrame> parse(const uint8_t* buffer, const size_t len){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Reived buffer with length: " + std::to_string(len));

        bool result = true;
        std::shared_ptr<UFrame> frame = std::shared_ptr<UFrame>(new UFrame());

        if(_debug){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " " + print_buff(buffer, len));
        }

        uint8_t tmp_buff[140];
        size_t r_len = decode(buffer, len, tmp_buff, sizeof(tmp_buff));

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
        else if((frame->is_RSTACK() || frame->is_ERROR()) && len != 6){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length (RSTACK, ERROR)");
            result = false;
        }
        else if(frame->is_DATA() && len < 7){
            logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Incorrect frame length (DATA)");
            result = false;
        }
        else if((frame->is_ACK() || frame->is_NAK()) && len != 5){
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
     */
    size_t encode(const std::shared_ptr<UFrame> frame, uint8_t* out_buff, const size_t len, const bool randm = false) const {
        size_t out_len = 0, i = 0;
        uint8_t tmp_buff[133];
        memset(tmp_buff, 0x00, sizeof(tmp_buff));

        /*
         4.1.0 for DATA
        */
       if(frame->is_DATA()){
           frame->set_frmNum(_info->frmNum());
           frame->set_ackNum(_info->ackNum());
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

    void randomize(uint8_t* buff, const size_t len) const {
        uint8_t rand = Rand0;
        for(int i = 0; i < len; i++){
            buff[i] ^= rand;
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


    /**
     * UART worker function
     *
     * 1. Wait until UART not connected to device (is_started)
     * 2. Initialize device
     * 2.1 Send RST frame
     * 2.2 Receive correct RSTACK frame
     * 2.3 If RSTACK frame will not receive during T_RSTACK_MAX repeat from 2.1
     * 2.4 Mark UART is on Connected state
     * 2.5 Notify EZSP
     * 3. Process wait for input data and process EZSP messages (send, receive answer)
     *
     */
    static void worker(ZBUart* p_uart){
        uint8_t buff[255];
        std::shared_ptr<ZBUart_Info> info = p_uart->get_info();

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + "Worker started.");

        auto fn = [p_uart, info]{return (p_uart->is_stop_signal() | info->is_started());};
        while(!p_uart->is_stop_signal()){

            {
                std::unique_lock<std::mutex> lk(p_uart->cv_m);
                p_uart->cv.wait(lk, fn);

                logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + std::string(" Signal detected Conneted: ") + std::to_string(info->is_started()));
            }

            if(p_uart->is_stop_signal()){
                logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + "Worker. Stop signal detected");
                break;
            }



        }

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + "Worker finished.");
    }


    /**
     * Initialize device
     */
    bool init_device(const int attempts = 3) {
        uint8_t buff[255];
        int fdnm, res;
        bool result = false;
        int attempt_cnt = 0;

        //create RST frame
        std::shared_ptr<UFrame> fr_rst = compose(ftype::RST);
        size_t wr_len = encode(fr_rst, buff, sizeof(buff), true);

        while(1){
            attempt_cnt++;
            if(attempt_cnt>=attempts){
                break;
            }

            res = write_data(buff, wr_len);
            if(!res){
                logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Write failed");
                break;
            }

            size_t b_len = sizeof(buff);
            res = read_data(buff, b_len);
            if(res!=0){ //error
                if(res == ETIME){ //Timeout - resend RST frame
                    continue;
                }
                break;
            }
            else{ //read some data finished by FlagByte or other end frame byte
                if(b_len==0){
                    continue;
                }
                std::shared_ptr<UFrame> fr_rsv = parse(buff, b_len);
                if(fr_rsv){
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Received: " + fr_rsv->to_string());

                    if(fr_rsv->is_RSTACK()){
                        //success
                    }
                }

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
                logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Wrote less Len: " + std::to_string(len) + " Real: " + std::to_string(res));
            }
        }
        return 0;
    }

    /**
     * Read data from device
     */
    int read_data(uint8_t* buffer, size_t& len, int tmout = 4){
        fd_set fd_read;
        struct timeval timeout;
        int fdnm = 1;

        size_t buff_len = len;
        while(1){
            FD_ZERO(&fd_read);
            FD_SET(_info->_fd, &fd_read);

            timeout.tv_sec = tmout;
            timeout.tv_usec = 0;

            int res = select(fdnm, &fd_read, NULL, NULL, &timeout);
            if(res == -1){
                int err = errno;
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Select error: " + std::to_string(err));
                if(err == EBADF){
                    //close connection and try reconnect again
                    disconnect();
                }
                return err;
            }

            //timeout. Generally speaking select does not return such code
            if(res == 0){
                return ETIME;
            }

            res = read(_info->_fd, buffer, buff_len);
            if(res == -1){
                int err = errno;
                logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Read error: " + std::to_string(err));
                if(err == EBADF){
                    //close connection and try reconnect again
                    disconnect();
                }
                return err;
            }

            //check do we read end of frame already
            if(res > 0){
                logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) +  " " + print_buff(buffer, res));

                for(int i=0; i<res; i++){
                    //if we received frame finished by Cancel or Subst byte - ignore it
                    if(ashrvd::CancelByte == buffer[i] || ashrvd::SubstByte == buffer[i]){
                        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + " Cancel or Subst detected Pos: " + std::to_string(i));
                        //TODO: process not the end of chunk
                        len = 0;
                        break;
                    }
                    else if(ashrvd::FlagByte == buffer[i]){
                        len = (i+1);
                        break;
                    }

                    //no frame end on this data chunk
                    assert(false);

                }
            }
            else
                len = 0;

            break;
        }

        return 0;
    }

private:

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
    std::shared_ptr<ZBUart_Info> get_info() {
        return _info;
    }

    /**
     * Print buffer. Debug purposes only
     */
    const std::string print_buff(const std::uint8_t* buff, size_t len) {
        std::string result;
        char dec[10];
        for(int i=0; i<len; i++){
            sprintf(dec, "0x%02X ", buff[i]);
            result += dec;
        }
        return result;
    }


private:
    std::shared_ptr<ZBUart_Info> _info = std::make_shared<ZBUart_Info>(); //session information

    bool _debug;
};

} //end namespace zb_uart

#endif
