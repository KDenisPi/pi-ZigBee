/**
 * ezsp_sm.h
 *
 *  EZSP SM
 *
 *  Created on: May 28, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_SM_H_
#define PI_ZIGBEE_LIB_EZSP_SM_H_

#include <cstdint>
#include <memory>

#include "CircularBuffer.h"
#include "ezsp_defs.h"

namespace zb_ezsp {

enum Ezsp_State : uint8_t {
    SM_Initial              = 0,    //Not started yet
    SM_UART_Connected       = 1,    //Starting UART
    SM_Get_Device_Info      = 2,    //UART starter. Get device info
    SM_Init_Network         = 3,    //Initialise network information (Form network or Join network)
    SM_UP_and_Ready         = 4     //Network UP ready to work
};

enum Ezsp_SM_Event : uint16_t {
    EVT_Start           = 0,
    EVT_UART_UP         ,
    EVT_CONF_FINISHED   ,
    EVT_NET_STATUS      ,
    EVT_NEIGHBOR_CONN   ,
    EVT_ERROR           ,
    EVT_TRUST_CENTER         //I am trust center now
};

/**
 *
 */
class EzspEvent {
public:
    EzspEvent(const Ezsp_SM_Event evt_id) :_evt_id(evt_id), _prm_byte(0), _status(EmberStatus::EMBER_SUCCESS) {

    }

    EzspEvent(const Ezsp_SM_Event evt_id, const uint8_t prm) :_evt_id(evt_id), _prm_byte(prm) {

    }

    EzspEvent(const Ezsp_SM_Event evt_id, const EmberStatus status, const uint8_t prm) :_evt_id(evt_id), _status(status), _prm_byte(prm) {

    }

    uint8_t get_prm() const {
        return _prm_byte;
    }

    const EmberStatus get_status() const {
        return _status;
    }

    const Ezsp_SM_Event id() const {
        return _evt_id;
    }

    const EId frame_id() const {
        return _frame_id;
    }

    void set_frame_id(const EId frame_id) {
        _frame_id = frame_id;
    }

    const std::string to_string() const {
        char buff[60];
        std::sprintf(buff, "Event: 0x%02X ID: 0x%02X Status: 0x%02X Prm: 0x%02X", _evt_id, _frame_id, _status, _prm_byte);

        return std::string(buff);
    }

protected:
    Ezsp_SM_Event _evt_id;
    uint8_t _prm_byte;
    EmberStatus _status;

    EId _frame_id;
};

using EventPtr = std::shared_ptr<EzspEvent>;
using EventBuff = piutils::circbuff::CircularBuffer<EventPtr>;


}

#endif