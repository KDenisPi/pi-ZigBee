/**
 * aps_defs.h
 *
 *  Application Support Sublayer (APS) Data structure definition
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_APS_DEFS_H_
#define PI_ZIGBEE_LIB_APS_DEFS_H_

#include <cstdint>

namespace zb_aps {

/**
 * APS Commands (4.4.1.3 Security Processing of APS Commands)
 */
using aps_cmd = enum APS_Command : uint8_t {
    Transport_Key   = 0x05,
    Update_Device   = 0x06,
    Remove_Device   = 0x07,
    Request_Key     = 0x08,
    Switch_Key      = 0x09,
    Tunnel_Data     = 0x0E,
    Verify_Key      = 0x0F,
    Confirm_Key     = 0x10
};

} //namespace zb_aps
#endif