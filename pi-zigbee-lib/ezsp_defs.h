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

#define VER_8 1
#ifdef VER_8
using id_type = uint16_t;
#else
using id_type = uint8_t;
#endif

using EZSP_Bool = uint8_t;
using EZSP_EzspConfigId = uint8_t;


using EId = enum EFrame_ID : id_type {
    ID_version = 2
};

}

#endif
