/**
 * ezsp_defs.h
 *
 *  EZSP Data structure definition
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_DEFS_H_
#define PI_ZIGBEE_LIB_EZSP_DEFS_H_

#include <cstdint>

namespace zb_ezsp {

/**
 * There is a new EZSP frame format in Release 6.7. Frame IDs have been expanded from 1 byte (8 bits) to 2 bytes (16 bits). Frame ID
 *   values are now shown with 4 hex digits to emphasize the larger Frame ID numbering space.
 */
//#define VER_8 1
#ifdef VER_8
using id_type = uint16_t;
#else
using id_type = uint8_t;
#endif

class EzspVersion {
public:
    EzspVersion() {}
    ~EzspVersion() {}

    static uint8_t _ver;
    static uint8_t ver(){
        return _ver;
    }

};

using EZSP_Bool = uint8_t;
using EZSP_EzspConfigId = uint8_t;


using EId = enum EFrame_ID : id_type {
    ID_version = 0x00,
    ID_Echo = 0x81
};

}

#endif
