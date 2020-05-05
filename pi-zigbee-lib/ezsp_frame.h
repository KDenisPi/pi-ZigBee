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
#include <string>

#include "ezsp_defs.h"
#include "ezsp_util.h"

namespace zb_ezsp {

/**
 * Version command ID 0x02
 * Request
 */
using ver_req = struct  __attribute__((packed, aligned(1))) ezsp_ver_req {
    uint8_t _ver;

    ezsp_ver_req& operator=(const ezsp_ver_req& obj){
        _ver = obj._ver;
        return *this;
    }
};

/**
 * Version command ID 0x02
 * Response
 */
using ver_resp = struct  __attribute__((packed, aligned(1))) ezsp_ver_resp {
    uint8_t _ver;           //protocolVersion The EZSP version the NCP is using (4).
    uint8_t _stackType;     //stackType The type of stack running on the NCP (2).
    uint16_t _stackVersion;  //stackVersion The version number of the stack.

    ezsp_ver_resp& operator=(const ezsp_ver_resp& obj){
        _ver = obj._ver;
        _stackType = obj._stackType;
        _stackVersion = obj._stackVersion;
        return *this;
    }
};


using EType = enum EFrameType : uint8_t {
    Command = 0x00,
    Response = 0x80
};

using ESleepMode = enum EFrame_SleepMode : uint8_t {
    Idle = 0x00,
    DeepSleep = 0x01,
    PowerDown = 0x02,
    Reserved = 0x03
};

using EOverflow = enum EFrame_Overflow_Bit : uint8_t {
    OvFl_No  = 0x00,   //0 No memory shortage since the previous response.
    OvFl_Yes = 0x01    //1 The NCP ran out of memory since the previous response.
};

using ETruncated = enum EFrame_Truncated_Bit : uint8_t {
    Trun_Yes = 0x02, // 1 The NCP truncated the current response to avoid exceeding the maximum EZSP frame length.
    Trun_No = 0x00   // 0 The current response was not truncated.
};

using ECallbackP = enum EFrame_Callback_Pending : uint8_t {
    CBack_Yes = 0x04,   //1 A callback is pending on the NCP. If this response is a callback, at least one more callback is available.
    CBack_No = 0x00     //0 All callbacks have been delivered to the host.
};

using ECallbackType = enum EFrame_Callback_Type : uint8_t {
    No_Callback = 0x00,   //This response is not a callback
    Async_Send = 0x01,    //This response is a synchronous callback. It was sent in response to a callback command.
    Async_no_Send = 0x02, //(UART interface only) This response is an asynchronous callback. It was not sent in response to a callback command.
    Callback_Reserved = 0x03       //Reserved
};

using EFormat = enum EFrame_Format_Version : uint8_t {
    Ver_0 = 0x00,
    Ver_1 = 0x01,
    Ver_R1 = 0x02,
    Ver_R2 = 0x03,
};

using EEnable = enum EFrame_Enable : uint8_t {
    EFrame_Padding_Enabled = (1 << 6),
    EFrame_Security_Enabled = (1 << 7),
    Mask = 0x03
};

template <typename T>
class EFrame {
public:
    /**
     * Frame data payload
     */
    struct  __attribute__((packed, aligned(1))) FrameData{
        uint8_t _seq;   //Sequence
        uint8_t _ctrl_low;  //Control byte (Low)
        uint8_t _ctrl_high; //Control byte (High)
        T _parm;        //Frame parameters payload
    } _data;

    EId _id;    //Frame ID

    /**
     * Response length
     */
    constexpr const size_t resp_length() const {
        return sizeof(FrameData);
    }

    const uint8_t seq() const{
        return _data._seq;
    }

    void set_seq(const uint8_t seq){
        _data._seq = seq;
    }

    const EId id() const {
        return _id;
    }

    void set_id(const EId id){
        _id = id;
    }

    void set_sleep_mode(const ESleepMode sm){
        _data._ctrl_low |= sm;
    }

    const uint8_t ctrl_low() const {
        return _data._ctrl_low;
    }

    const uint8_t ctrl_high() const {
        return _data._ctrl_high;
    }

    const ESleepMode sleep_mode() const {
        assert((_data._ctrl_low & EType::Response) == 0);
        return (ESleepMode)(_data._ctrl_low & EEnable::Mask);
    }

    bool is_respose() const {
        return ((_data._ctrl_low&EType::Response)>0 ? true : false);
    }

    bool is_overflow() const {
        return ((_data._ctrl_low&EOverflow::OvFl_Yes)>0 ? true : false);
    }

    bool is_truncated() const {
        return ((_data._ctrl_low&ETruncated::Trun_Yes)>0 ? true : false);
    }

    bool is_callback_pending() const {
        return ((_data._ctrl_low&ECallbackP::CBack_Yes)>0 ? true : false);
    }

    ECallbackType callback_type() const {
        uint16_t res = (_data._ctrl_low >> 3);
        return (ECallbackType)(res & EEnable::Mask);
    }

    uint16_t network_index() const {
        return ((_data._ctrl_low >> 5) & EEnable::Mask);
    }

    void set_network_index(const uint8_t nidx){
        _data._ctrl_low |= ((nidx & EEnable::Mask) << 5);
    }

    EFormat frame_format_ver() const {
        return (EFormat)(_data._ctrl_high & EEnable::Mask );
    }

    bool is_padding_enabled() const {
        return ((_data._ctrl_high & EEnable::EFrame_Padding_Enabled) > 0);
    }

    bool is_security_enabled() const {
        return ((_data._ctrl_high & EEnable::EFrame_Security_Enabled) > 0);
    }

    /**
     * Prepare data for sendig
     */
    size_t put(uint8_t* buff, size_t pos){
        pos = Conv::put(buff, pos, _data._seq);
        pos = Conv::put(buff, pos, _data._ctrl_low);
        pos = Conv::put(buff, pos, _data._ctrl_high);   //TODO: Set extended byte for ver 5
        pos = Conv::put(buff, pos, (id_type)_id);
        pos = put(_data._parm, buff, pos);
        return pos;
    }

    bool load(const uint8_t* buff, size_t len){
        if(len<resp_length()){
            return true;
        }

        size_t pos = 0;
        _data._seq = Conv::get_byte(buff, pos);     //Sequence number
        _data._ctrl_low = Conv::get_byte(buff, pos);    //Control low
        _data._ctrl_high = Conv::get_byte(buff, pos);   //Control high
        if(_data._ctrl_high == 0xFF && (EzspVersion::ver() >= 5 && EzspVersion::ver() < 8)){ //version 5
            _data._ctrl_high = Conv::get_byte(buff, pos);   //Control high, extended
        }
        get(_data._parm, buff, pos);
    }

    /**
     * Version
     */
    size_t put(ver_req& param, uint8_t* buff, size_t pos){
        return Conv::put(buff, pos, param._ver);
    }

    void get(ver_resp& param, const uint8_t* buff, size_t pos){
        param._ver = Conv::get_byte(buff, pos);
        param._stackType = Conv::get_byte(buff, pos);
        param._stackVersion = Conv::get_short(buff, pos);
    }


public:
    /**
     * Default constructor for a new Frame creating
     */
    EFrame(const EId id, const T& params) {
        set_id(id);
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
        char buffer[32];

        sprintf(buffer, "Control Low: 0x%X High: 0x%X ", ctrl_low(), ctrl_high());
        result = buffer;
        result += "Sequence: " + std::to_string(seq());
        if(is_respose()){
            result += " [Response] Overflow: " + std::to_string(is_overflow()) + " Truncated: " +
                std::to_string(is_truncated()) + " Callback Pending: " +
                std::to_string(is_callback_pending()) + " Type: " +
                std::to_string(callback_type()) + " Net Index: " + std::to_string(network_index());
        }
        else{
            result += " [Command] ID: " + std::to_string(id()) + " Sleep Mode: " + std::to_string(sleep_mode()) +  " Net Index: " + std::to_string(network_index());
        }

        result += " Format ver: " + std::to_string(frame_format_ver()) + " Padding: " + std::to_string(is_padding_enabled()) + " Security: " + std::to_string(is_security_enabled());
        return result;
    }

};

}
#endif