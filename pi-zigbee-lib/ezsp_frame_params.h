/**
 * ezsp_frame_params.h
 *
 *  EZSP Frames. Parameters
 *
 *  Created on: May 6, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_FRAME_PARAMS_H_
#define PI_ZIGBEE_LIB_EZSP_FRAME_PARAMS_H_

#include <cstdint>
#include <string>
#include "ezsp_defs.h"

namespace zb_ezsp {

/**
 * Version command ID 0x02
 * Request
 */
using ver_req = struct ezsp_ver_req {
    uint8_t _ver;
};

/**
 * Version. Response
 */
using ver_resp = struct ezsp_ver_resp {
    uint8_t _ver;           //protocolVersion The EZSP version the NCP is using (4).
    uint8_t _stackType;     //stackType The type of stack running on the NCP (2).
    uint16_t _stackVersion;  //stackVersion The version number of the stack.

    const std::string to_string() const {
        std::string result = "Version: " + std::to_string((uint16_t)_ver) + " Stack Type: " + std::to_string((uint16_t)_stackType) + " Version: " + std::to_string(_stackVersion);
        return result;
    }
};

/**
 * Echo command ID 0x81
 *
 * Description: Variable length data from the Host is echoed back by the NCP. This command has no other effects and is designed for testing the link between the Host and NCP.
 */
using echo = struct ezsp_echo { //__attribute__((packed, aligned(1)))
    uint8_t dataLength;     // The length of the data parameter in bytes.
    uint8_t data[10];         // The data to be echoed back.

    const std::string to_string() const {
        std::string result = "Ping Length: " + std::to_string(dataLength);
        return result;
    }
};

}
#endif