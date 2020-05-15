/**
 * uart_fame.h
 *
 *  UART Frames
 *
 *  Created on: Apr 2, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_UART_FRAME_H_
#define PI_ZIGBEE_LIB_UART_FRAME_H_

#include <memory>
#include <string.h>
#include <assert.h>

#include "logger.h"
#include "crc16.h"

namespace zb_uart {

/**
 * UART Frame implementation
 *
*	@ft: Frame type
*	@data: pointer to frame data
*	@len: number of bytes in the frame data
*
 */

using ftype = enum FrameType : uint8_t {
    DATA = 0x00,    //Purpose: Carries all EZSP frames
    NO_DATA = 0x80,
    ACK = 0x80,     //Purpose: Acknowledges receipt of a valid DATA frame
    NAK = 0xA0,     //Purpose: Indicates receipt of a DATA frame with an error or that was discarded due to lack of memory
    RST = 0xC0,     //Purpose: Requests the NCP to perform a software reset (valid even if the NCP is in the FAILED state)
    RSTACK = 0xC1,  //Purpose: Informs the Host that the NCP has reset and the reason for the reset
    ERROR = 0xC2};  //Purpose: Informs the Host that the NCP detected a fatal error and is in the FAILED state

/**
 * 0x7E Flag Byte: Marks the end of a frame.
 *   When a Flag Byte is received, the data received since the last Flag Byte or Cancel Byte is tested to see whether it is a valid frame.
 * 0x7D Escape Byte: Indicates that the following byte is escaped.
    If the byte after the Escape Byte is not a reserved byte, bit 5 of the byte is complemented to restore its original value. If the
    byte after the Escape Byte is a reserved value, the Escape Byte has no effect.
* 0x11 XON: Resume transmission Used in XON/XOFF flow control. Always ignored if received by the NCP.
* 0x13 XOFF: Stop transmission Used in XON/XOFF flow control. Always ignored if received by the NCP.
* 0x18 Substitute Byte: Replaces a byte received with a low-level communication error (e.g., framing error) from the UART.
        When a Substitute Byte is processed, the data between the previous and the next Flag Bytes is ignored.
* 0x1A Cancel Byte: Terminates a frame in progress.
    A Cancel Byte causes all data received since the previous Flag Byte to be ignored. Note that as a special case, RST and
    RSTACK frames are preceded by Cancel Bytes to ignore any link startup noise
*/
using ashrvd = enum ASH_Reserved : uint8_t {FlagByte = 0x7E, EscapeByte = 0x7D, XON = 0x11, XOFF = 0x13, SubstByte = 0x18, CancelByte = 0x1A};

/**
 * Reset and error codes
 *
0x00 Reset: Unknown reason
0x01 Reset: External
0x02 Reset: Power-on
0x03 Reset: Watchdog
0x06 Reset: Assert
0x09 Reset: Boot loader
0x0B Reset: Software
0x51 Error: Exceeded maximum ACK timeout count
0x80 Chip-specific error reset code
*/
using codes = enum ASH_ResetErrorCodes : uint8_t {RST_Unknown = 0x00, RST_External = 0x01, RST_PowerOn = 0x02, RST_Watchdog = 0x03, RST_Assert = 0x06, RST_BootLoader = 0x09, RST_Software = 0x0B,
                                                    ERR_Max_ACK_Timeout = 0x51, ERR_Reset = 0x80};

class ZBUart;

class UFrame {
public:
    friend ZBUart;

    static const uint8_t BEscape = 0x7D;

    //save CRC
    void crc(const uint16_t crc) {
        this->_crc = crc;
    }

    //Get CRC value
    const uint16_t get_crc() const {
        return _crc;
    }

    //Return Flag Byte
    const u_int8_t get_flag_byte() const {
        return _fb;
    }

    void set_flag_byte(const uint8_t fb){
        _fb = fb;
    }

    /**
     * Set control byte
     */
    void set_control_byte(const uint8_t cbyte){
        _cb = cbyte;
    }

    uint8_t control_byte() const {
        return _cb;
    }

    //Check id this frate has DATA type
    bool inline is_DATA() const {
        return ((_cb & ftype::NO_DATA) == 0);
    }

    //Check if this frame has ACK type
    bool inline is_ACK() const {
        return ((_cb & 0xE0) == ftype::ACK);
    }

    //Check if this frame has NAK type
    bool inline is_NAK() const {
        return ((_cb & 0xE0) == ftype::NAK);
    }

    //Check if this frame has RST type
    bool inline is_RST() const {
        return (_cb == ftype::RST);
    }

    //Check if this frame has RSTACK type
    bool inline is_RSTACK() const {
        return (_cb == ftype::RSTACK);
    }

    //Check if this frame has ERROR type
    bool inline is_ERROR() const {
        return (_cb == ftype::ERROR);
    }

    const size_t inline data_len() const {
        return _len;
    }

    //return frame length
    // 1- control byte
    // n - data (0-128)
    // 2 - CRC
    // 1 - Flag Byte
    const inline size_t flen() const{
        return (4+data_len());
    }

    //Get DATA frame sequence number
    uint8_t inline frmNum() const {
        assert(is_DATA());

        return ((_cb >> 4) & 0x07);
    }

    //Set DATA frame sequence number (0-7)
    void inline set_frmNum(const uint8_t seq_num){
        assert(seq_num<=7);
        assert(is_DATA());

        _cb = (_cb & 0x70) | (seq_num << 4);
        logger::log(logger::LLOG::DEBUG, "uframe", std::string(__func__) + " Control byte: " + std::to_string(_cb) + " seq_num: " + std::to_string(seq_num));
    }

    //Return pointer to frame data payload (nullptr for frames without data: ACK, NAk, RST)
    const uint8_t* data() const {
        assert(is_DATA() || is_RSTACK() || is_ERROR());
        return _data;
    }

    //ackNum - acknowledges receipt of DATA frames up to, but not including, ackNum
    uint8_t ackNum() const {
        assert(is_DATA() || is_ACK() || is_NAK());

        return (_cb & 0x07);
    }

    void set_ackNum(const uint8_t ackNum){
        assert(ackNum<=7);
        assert(is_DATA() || is_ACK() || is_NAK());

        _cb = (_cb & 0xF8) | (ackNum & 0x07);

        logger::log(logger::LLOG::DEBUG, "uframe", std::string(__func__) + " Control byte: " + std::to_string(_cb) + " ackNum: " + std::to_string(ackNum));
    }

    //reTx - set to 1 in a retransmitted DATA frame; 0 otherwise
    bool inline reTx() const {
        assert(is_DATA());

        return ((_cb & 0x08) != 0);
    }

    void inline set_reTx(){
        assert(is_DATA());

        _cb |= 0x08;
    }

    //nRdy – host sets to 1 if to inhibit the NCP from sending callbacks frames to the host (always 0 in frames sent by the NCP)
    void inline set_nRdy() {
        assert(is_ACK() || is_NAK());

        _cb |= 0x08;
    }

    const uint8_t nRdy() const {
        assert(is_ACK() || is_NAK());

        return ((_cb >> 3) & 0x01);
    }


    //Return frame type name
    const std::string get_type()const {
        if(is_DATA()) return ftype2str(ftype::DATA);
        if(is_ACK()) return ftype2str(ftype::ACK);
        if(is_NAK()) return ftype2str(ftype::NAK);
        if(is_RST()) return ftype2str(ftype::RST);
        if(is_RSTACK()) return ftype2str(ftype::RSTACK);
        if(is_ERROR()) return ftype2str(ftype::ERROR);

        return std::string("UNKNOWN");
    }


    //Return frame type name
    static const std::string ftype2str(const ftype ft){
        switch(ft){
            case ftype::ACK: return std::string("ACK");
            case ftype::DATA: return std::string("DATA");
            case ftype::ERROR: return std::string("ERROR");
            case ftype::NAK: return std::string("NAK");
            case ftype::RST: return std::string("RST");
            case ftype::RSTACK: return std::string("RSTACK");
        }

        return std::string("UNKNOWN");
    }


    //Print frame information
    const std::string to_string() {
        char buffer[256];

        sprintf(buffer, " CB:0x%02X Type:%s, Len Full:%lu Data:%lu CRC:0x%02X FB:0x%02X", this->control_byte(), this->get_type().c_str(), this->flen(), this->data_len(), this->get_crc(), this->get_flag_byte());
        std::string result = buffer;
        if(is_DATA()){
            sprintf(buffer, " ftmNum:%02d reTx:%02d ackNum: %02d", this->frmNum(), this->reTx(), this->ackNum());
            result += buffer;
        }
        else if(is_ACK() || is_NAK()){
            sprintf(buffer, " nRdy:%02d ackNum: %02d", this->nRdy(), this->ackNum());
            result += buffer;
        }
        else if(is_RSTACK() && data_len()==2)
        {
            sprintf(buffer, " Version:%02X Reset code:%02X", _data[0], _data[1]);
            result += buffer;
        }

        if(this->data_len() > 0)
            result += "\nDATA: ";

        for(int i=0; i<this->data_len(); i++){
            sprintf(buffer, "0x%02X ", _data[i]);
            result += buffer;
        }

        return result;
    }

    /**
     * Destructor
     */
    ~UFrame(){
        logger::log(logger::LLOG::DEBUG, "uframe", std::string(__func__) + " Control byte: " + std::to_string(_cb));
    }

protected:
    //
    UFrame() : _len(0), _cb(0x00) {
    }

    //
    UFrame(const ftype ft) : _len(0), _cb(0x00) {
        set_type(ft);
    }

    /**
     * Set frame type
     */
    void set_type(const ftype ft){
        if(ft == ftype::DATA){
            _cb &= ~(ftype::NO_DATA);
        }
        else{
            _cb |= ft;
        }
    }

    /**
     * Load data to frame body
     */
    void set_data(const uint8_t* data, size_t len){
        assert(len <= 128);

        if(len > 0)
            memcpy(&_data[0], data, len);
        _len = len;
    }

    uint8_t* raw_data() {
        return _data;
    }

    //Set error information
    void set_error(uint16_t err, const std::string& err_str){
        _error = err;
        _error_str = err_str;
    }

    uint16_t error() const {
        return _error;
    }

    const std::string error_ste() const {
        return _error_str;
    }

private:
    uint8_t _cb;            //Control byte
    uint8_t _data[129];     //Frame data
    size_t _len;            //Frame data length
    uint16_t _crc;          //CRC
    uint8_t _fb;            //Flag byte

    uint16_t _error; //last error code
    std::string _error_str; //last error description
};

}//namespace zb_uart
#endif