/**
 * ezsp_fame.h
 *
 *  EZSP Frames
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_FRAME_H_
#define PI_ZIGBEE_LIB_EZSP_FRAME_H_

#include <cstdint>
#include "ezsp_util.h"

namespace zb_ezsp {

using ver_req = struct  __attribute__((packed, aligned(1))) ezsp_req_ver {
    uint8_t _ver;

    ezsp_req_ver& operator=(const ezsp_req_ver& obj){
        _ver = obj._ver;
        return *this;
    }

    size_t put(uint8_t* buff, size_t pos){
        return Conv::put(buff, pos, _ver);
    }

    void get(const uint8_t* buff, size_t& pos){
        _ver = Conv::get_byte(buff, pos);
    }
};

#define VER_8 1
#ifdef VER_8
using id_type = uint16_t;
#else
using id_type = uint8_t;
#endif

using EType = enum EFrameType : uint16_t {
    Command = 0x0000,
    Response = 0x0080
};

using ESleepMode = enum EFrame_SleepMode : uint16_t {
    Idle = 0x0000,
    DeepSleep = 0x0001,
    PowerDown = 0x0002,
    Reserved = 0x0003
};

using EOverflow = enum EFrame_Overflow_Bit : uint16_t {
    OvFl_No  = 0x0000,   //0 No memory shortage since the previous response.
    OvFl_Yes = 0x0001    //1 The NCP ran out of memory since the previous response.
};

using ETruncated = enum EFrame_Truncated_Bit : uint16_t {
    Trun_Yes = 0x0002, // 1 The NCP truncated the current response to avoid exceeding the maximum EZSP frame length.
    Trun_No = 0x0000   // 0 The current response was not truncated.
};

using ECallbackP = enum EFrame_Callback_Pending : uint16_t {
    CBack_Yes = 0x0004,   //1 A callback is pending on the NCP. If this response is a callback, at least one more callback is available.
    CBack_No = 0x0000     //0 All callbacks have been delivered to the host.
};

using ECallbackType = enum EFrame_Callback_Type : uint16_t {
    No_Callback = 0x0000,   //This response is not a callback
    Async_Send = 0x0001,    //This response is a synchronous callback. It was sent in response to a callback command.
    Async_no_Send = 0x0002, //(UART interface only) This response is an asynchronous callback. It was not sent in response to a callback command.
    Callback_Reserved = 0x0003       //Reserved
};

using EFormat = enum EFrame_Format_Version : uint16_t {
    Ver_0 = 0x0000,
    Ver_1 = (0x0001 << 8),
    Ver_R1 = (0x0002 << 8),
    Ver_R2 = (0x0003 << 8),
};

uint16_t EFrame_Padding_Enabled = (1 << 14);
uint16_t EFrame_Security_Enabled = (1 << 15);

template <typename T>
class EFrame {
public:
    /**
     * Frame data payload
     */
    struct  __attribute__((packed, aligned(1))) FrameData{
        uint8_t _seq;   //Sequence
        uint16_t _ctrl;  //Control byte
        id_type _id;    //Frame ID
        T _parm;        //Frame parameters payload
    } _data;

    constexpr const size_t length() const {
        return sizeof(FrameData);
    }

    void set_sleep_mode(const ESleepMode sm){
        _data._ctrl |= sm;
    }

    const ESleepMode sleep_mode() const {
        assert((_data._ctrl&EType::Response)==0);
        return (_data._ctrl&0x0003);
    }

    bool is_respose() const {
        return ((_data._ctrl&EType::Response)>0 ? true : false);
    }

    bool is_overflow() const {
        return ((_data._ctrl&EOverflow::OvFl_Yes)>0 ? true : false);
    }

    bool is_truncated() const {
        return ((_data._ctrl&ETruncated::Trun_Yes)>0 ? true : false);
    }

    bool is_callback_pending() const {
        return ((_data._ctrl&ECallbackP::CBack_Yes)>0 ? true : false);
    }

    ECallbackType callback_type() const {
        return ((_data._ctrl >> 3) & 0x0003);
    }

    uint16_t network_index() const {
        return ((_data._ctrl >> 5) & 0x0003);
    }

    void set_network_index(const uint16_t nidx){
        _data._ctrl |= ((nidx&0x0003) << 5);
    }

    EFormat frame_format_ver() const {
        return (_data._ctrl & (0x0003<<8));
    }

    bool is_padding_enabled() const {
        return ((_data._ctrl&EFrame_Padding_Enabled) > 0);
    }

    bool is_security_enabled() const {
        return ((_data._ctrl&EFrame_Security_Enabled) > 0);
    }

    /**
     * Prepare data for sendig
     */
    size_t put(uint8_t* buff, size_t pos){
        pos = Conv::put(buff, pos, _data._seq);
        pos = Conv::put(buff, pos, _data._ctrl);
        pos = Conv::put(buff, pos, _data._id);
        pos = _data._parm.put(buff, pos);
        return pos;
    }

    bool load(const uint8_t* buff, size_t len){
        if(len<length()){
            return true;
        }

        size_t pos = 0;
        _data._seq = Conv::get_byte(buff, pos);
        _data._ctrl = Conv::get_short(buff, pos);
        _data._id = Conv::get_short(buff, pos);
        _data._parm.get(buff, pos);
    }

public:
    /**
     * Default constructor for a new Frame creating
     */
    EFrame(const T& params) {
        this->_data._parm = params;
    }

    /**
     * Constructor for loading data from the buffer
     */
    EFrame(){

    }

    EFrame(EFrame&&) = delete;
    EFrame& operator=(const EFrame&) = delete;
    EFrame& operator=(EFrame&&) = delete;

    const std::string to_string() const {
        std::string result;
        if(is_respose()){
            result = "Response Overflow: " + is_overflow() + " Truncated: " + is_truncated() + " Callback Pending: " + is_callback_pending() + " Type: " + callback_type() + " Net Index: " + network_index();
        }
        else{
            result = "Command Sleep Mode: " + sleep_mode() +  " Net Index: " + network_index();
        }

        result += " Format ver: " + frame_format_ver() + " Padding: " + is_padding_enabled() + " Security: " + is_security_enabled();
        return result;
    }

};

}
#endif