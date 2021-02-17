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
#include <memory>
#include <assert.h>

#include "ezsp_defs.h"
#include "ezsp_util.h"
#include "ezsp_frame_params.h"

namespace zb_ezsp {

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

class EFrame {
public:

    static const uint8_t max_buffer_len = 132;
    const size_t free_len(const size_t pos) const {
        return (max_buffer_len - pos);
    }

    /**
     * Frame data payload
     */
    using FData = struct  FrameData{ //__attribute__((packed, aligned(1)))
        uint8_t _seq;   //Sequence
        uint8_t _ctrl_low;  //Control byte (Low)
        uint8_t _ctrl_high; //Control byte (High)
        EId _id;        //Frame ID
    };

    std::shared_ptr<FData> _data;    //Frame data

    const uint8_t seq() const{
        return _data->_seq;
    }

    void set_seq(const uint8_t seq){
        _data->_seq = seq;
    }

    const EId id() const {
        return _data->_id;
    }

    void set_id(const EId id){
        _data->_id = id;
    }

    void set_sleep_mode(const ESleepMode sm){
        _data->_ctrl_low |= sm;
    }

    const uint8_t ctrl_low() const {
        return _data->_ctrl_low;
    }

    const uint8_t ctrl_high() const {
        return _data->_ctrl_high;
    }

    const ESleepMode sleep_mode() const {
        assert((_data->_ctrl_low & EType::Response) == 0);
        return (ESleepMode)(_data->_ctrl_low & EEnable::Mask);
    }

    bool is_respose() const {
        return ((_data->_ctrl_low&EType::Response)>0 ? true : false);
    }

    bool is_overflow() const {
        return ((_data->_ctrl_low&EOverflow::OvFl_Yes)>0 ? true : false);
    }

    bool is_truncated() const {
        return ((_data->_ctrl_low&ETruncated::Trun_Yes)>0 ? true : false);
    }

    bool is_callback_pending() const {
        return ((_data->_ctrl_low&ECallbackP::CBack_Yes)>0 ? true : false);
    }

    ECallbackType callback_type() const {
        uint16_t res = (_data->_ctrl_low >> 3);
        return (ECallbackType)(res & EEnable::Mask);
    }

    const uint8_t network_index() const {
        return ((_data->_ctrl_low >> 5) & EEnable::Mask);
    }

    void set_network_index(const uint8_t nidx = 0){
        _data->_ctrl_low |= ((nidx & EEnable::Mask) << 5);
    }

    EFormat frame_format_ver() const {
        return (EFormat)(_data->_ctrl_high & EEnable::Mask );
    }

    bool is_padding_enabled() const {
        return ((_data->_ctrl_high & EEnable::EFrame_Padding_Enabled) > 0);
    }

    bool is_security_enabled() const {
        return ((_data->_ctrl_high & EEnable::EFrame_Security_Enabled) > 0);
    }

    const uint8_t legasyFrameID = 0xFF; //Legacy Frame ID

    /**
     * Prepare data for sendig
     */
    template<typename T>
    size_t put(uint8_t* buff, size_t pos, const T& params){
        pos = Conv::put(buff, pos, _data->_seq);
        pos = Conv::put(buff, pos, _data->_ctrl_low);
        if(EzspVersion::ver() >= 5){ //version 5
            pos = Conv::put(buff, pos, legasyFrameID);
            pos = Conv::put(buff, pos, _data->_ctrl_high);
        }
        pos = Conv::put(buff, pos, (id_type)_data->_id);
        pos = put_param(params, buff, pos);
        return pos;
    }

    template<typename T>
    const std::shared_ptr<T> load(const uint8_t* buff, size_t len){
        size_t pos = 0;
        pos = Conv::get(buff, pos, _data->_seq);     //Sequence number
        pos = Conv::get(buff, pos, _data->_ctrl_low);    //Control low

        if(EzspVersion::ver() >= 5 && buff[pos] == 0xFF){ //version 5
            pos++;
            pos = Conv::get(buff, pos, _data->_ctrl_high);   //Control high
        }
        //Get Frame ID (uint8 for versions before 8, uint16 8 and higher)
        _data->_id = Conv::get_id(buff, pos);

        //load parameters
        T* params = new T();
        get_param(*params, buff, pos);
        return std::shared_ptr<T>(params);
    }

    size_t get_param(EmberEUI64& param, const uint8_t* buff, size_t& pos);
    size_t get_param(Eui64& param, const uint8_t* buff, size_t& pos);
    size_t get_param(NodeId& param, const uint8_t* buff, size_t& pos);
    size_t put_param(const NodeId& param, uint8_t* buff, size_t pos);
    size_t get_param(uint8t_value& param, const uint8_t* buff, size_t& pos);

    /**
     * Version
     */
    size_t put_param(const ver_req& param, uint8_t* buff, size_t pos);
    size_t get_param(ver_resp& param, const uint8_t* buff, size_t& pos);

    /**
     * Echo
     */
    size_t put_param(const data_array& param, uint8_t* buff, size_t pos);
    size_t get_param(data_array& param, const uint8_t* buff, size_t& pos);

    /**
     * Request without any parameters and response with Ember or Ezsp status only
     */

    size_t put_param(const zb_ezsp::ezsp_no_params& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::uint8t_value& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::ezsp_no_params& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::ezsp_EmberStatus& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::ezsp_EzspStatus& param, const uint8_t* buff, size_t& pos);

    /**
     * Start scan and another network related calls
     */
    size_t put_param(const zb_ezsp::start_scan& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::energyScanResultHandler& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::EmberZigbeeNetwork& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::networkFoundHandler& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::scanCompleteHandler& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::networkState& param, const uint8_t* buff, size_t& pos);

    size_t put_param(const zb_ezsp::EmberNetworkInitStruct& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::EmberNetworkParameters& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::permitJoining& param, uint8_t* buff, size_t pos);

    size_t get_param(zb_ezsp::EmberNetworkParameters& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::getNetworkParameters_resp& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::childJoinHandler& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::trustCenterJoinHandler& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::stackTokenChangedHandler& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::getParentChildParameters& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::lookupEui64ByNodeId& param, const uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::EmberRouteTableEntry& param, const uint8_t* buff, size_t pos);


    /**
     * NCP Value
     */
    size_t put_param(const zb_ezsp::value_set& param, uint8_t* buff, size_t pos);

    size_t put_param(const zb_ezsp::value_get_req& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::value_get_resp& param, const uint8_t* buff, size_t& pos);

    /**
     * Configuration value
     */
    size_t put_param(const zb_ezsp::configid_set_req& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::ezsp_configid_get_req& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::ezsp_configid_get_resp& param, const uint8_t* buff, size_t& pos);

    /**
     * Security
     */
    size_t put_param(const zb_ezsp::EmberInitialSecurityState& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::EmberCurrentSecurityState& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::EmberCurrentSecurityState& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::getCurrentSecurityState& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::EmberKeyStruct& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::getKey& param, const uint8_t* buff, size_t& pos);
    size_t put_param(const zb_ezsp::BecomeTrustCenter& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::unicastNwkKeyUpdate& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::keyData& param, uint8_t* buff, size_t pos);

    /**
     * APS Frame & messaging
     */
    size_t put_param(const zb_ezsp::EmberApsFrame& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::EmberApsFrame& param, const uint8_t* buff, size_t& pos);

    size_t put_param(const zb_ezsp::sendUnicast& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::sendUnicast_resp& param, const uint8_t* buff, size_t& pos);

    size_t get_param(zb_ezsp::messageSentHandler& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::incomingMessageHandler& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::incomingRouteErrorHandler& param, const uint8_t* buff, size_t& pos);

    size_t get_param(zb_ezsp::Route& param, const uint8_t* buff, size_t& pos);
    size_t get_param(zb_ezsp::getChildData& param, const uint8_t* buff, size_t& pos);

    size_t put_param(const zb_ezsp::setExtendedTimeout& param, uint8_t* buff, size_t pos);
    size_t put_param(const zb_ezsp::getExtendedTimeout& param, uint8_t* buff, size_t pos);

    size_t get_param(zb_ezsp::ZigbeeKeyEstablishmentHandler& param, const uint8_t* buff, size_t& pos);


    /**
     * Get data by Index
     */
    size_t put_param(const zb_ezsp::get_by_index& param, uint8_t* buff, size_t pos);

    /**
     * Binfing
     */
    size_t put_param(const zb_ezsp::EmberBindingTableEntry& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::EmberBindingTableEntry& param, const uint8_t* buff, size_t& pos);

    size_t put_param(const zb_ezsp::setBinding_req& param, uint8_t* buff, size_t pos);
    size_t get_param(zb_ezsp::getBinding& param, const uint8_t* buff, size_t& pos);



public:
    /**
     * Default constructor for a new Frame creating
     */
    EFrame(const EId id) {
        _data = std::make_shared<FData>();
        set_id(id);
    }

    /**
     * Constructor for loading data from the buffer
     */
    EFrame(){
        _data = std::make_shared<FData>();
    }

    EFrame(EFrame&& other) {
        this->_data = std::move(other._data);
    }

    EFrame& operator=(EFrame&& other){
        this->_data = std::move(other._data);
        return *this;
    }

    EFrame& operator=(const EFrame&) = delete;

    /**
     * Put frame data to string
     * Debug purpose only
     */
    const std::string to_string() const {
        std::string result;
        char buffer[32];

        sprintf(buffer, " Control Low: 0x%X High: 0x%X ", ctrl_low(), ctrl_high());
        result = buffer;
        result += "Sequence: " + std::to_string(seq());
        if(is_respose()){
            result += " [Response] ID: " + std::to_string(id()) +
                " Overflow: " + std::to_string(is_overflow()) +
                " Truncated: " + std::to_string(is_truncated()) +
                " Callback Pending: " + std::to_string(is_callback_pending()) +
                " Type: " + std::to_string(callback_type()) +
                " Net Index: " + std::to_string(network_index());
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