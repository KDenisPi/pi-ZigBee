/**
 * ezsp_defs_Rf4ce.h
 *
 *  EZSP Data structure definition (RF4CE Frames)
 *
 *  Created on: Jul 8, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_DEFS_RF4CE_H_
#define PI_ZIGBEE_LIB_EZSP_DEFS_RF4CE_H_

#include <cstdint>
#include "ezsp_defs.h"

namespace zb_ezsp {

using EmberRf4ceApplicationCapabilities = uint8_t;

/**
 * The RF4CE vendor information block.
 */
using EmberRf4ceVendorInfo = struct ezsp_EmberRf4ceVendorInfo {
    uint16_t vendorId; // The vendor identifier field shall contain the vendor identifier of the node.
    uint8_t vendorString[7]; // The vendor string field shall contain the vendor string of the node.
};

/**
 * The RF4CE application information block.
 */
using EmberRf4ceApplicationInfo = struct ezsp_EmberRf4ceApplicationInfo {
    EmberRf4ceApplicationCapabilities capabilities; // The application capabilities field shall contain information relating to the capabilities of the application of the node.
    uint8_t userString[15]; //  The user string field shall contain the user specified identification string.
    uint8_t deviceTypeList[3]; // The device type list field shall contain the list of device types supported by the node.
    uint8_t profileIdList[7]; // The profile ID list field shall contain the list of profile identifiers disclosed as supported by the node.
};

}

#endif
