/**
 * ezsp_types.h
 *
 *  EZSP Data types definition
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_TYPES_H_
#define PI_ZIGBEE_LIB_EZSP_TYPES_H_

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

using EZSP_Bool         = uint8_t;
using EZSP_EzspConfigId = uint8_t;
using EmberNodeId       = uint16_t;                 // 16-bit ZigBee network address.
using EmberEUI64        = uint8_t[8];               // EUI 64-bit ID (an IEEE address)
using EmberKeyData      = uint8_t[16];              // A 128-bit key contents EmberCertificateData
using ExtendedPanId     = uint8_t[8];               // The network's extended PAN identifier.

using EmberSecurityBitmask  = uint16_t;
using EmberKeyStructBitmask = uint16_t;
using EmberKeyType          = uint8_t;

/**
 * 2.3.2.3.6 MAC Capability Flags Field
 *
 */
class MACCapabilityFlags {
public:
    MACCapabilityFlags(const uint8_t flags = 0x00) : _flags(flags) {};
    void set(const uint8_t flags) {_flags = flags;}

    /**
     * The alternate PAN coordinator sub-field is one bit in length and shall be set to 1 if this node is capable of becoming a PAN coordinator. Otherwise, the alternative PAN coordinator sub-field shall be set to 0.
     */
    static const uint8_t Flag_Alternate_PAN_coordinator = 0x01;
    /**
     * The device type sub-field is one bit in length and shall be set to 1 if this node is a full function device (FFD).
     * Otherwise, the device type sub-field shall be set to 0, indicating a reduced function device (RFD).
     */
    static const uint8_t Flag_Full_function_device = (1<<1);

    /**
     * The power source sub-field is one bit in length and shall be set to 1 if the current power source is mains power.
     * Otherwise, the power source sub-field shall be set to 0. This information is derived from the node current power source field of the node power descriptor.
     */
    static const uint8_t Flag_Power_source  = (1<<2);

    /**
     * The receiver on when idle sub-field is one bit in length and shall be set to 1 if the device does not disable its
     * receiver to conserve power during idle periods. Otherwise, the receiver on when idle sub-field shall be set to 0 (see also section 2.3.2.4.)
     */
    static const uint8_t Flag_Receiver_on_when_idle = (1<<3);

    /**
     * The security capability sub-field is one bit in length and shall be set to 1 if the device is capable of sending
     * and receiving frames secured using the security suite specified in [B1]. Otherwise, the security capability sub-field shall be set to 0.
     */
    static const uint8_t Flag_Security_capability = (1<<6);

    /**
     * The allocate address sub-field is one bit in length and shall be set to 0 or 1.
     */
    static const uint8_t Flag_Allocate_address = (1<<7);

    const bool is_Alternate_PAN_coordinator() const {
        return (_flags & Flag_Alternate_PAN_coordinator) != 0;
    }

    const bool is_FFD() const {
        return (_flags & Flag_Full_function_device) != 0;
    }

    const bool is_Power_source() const {
        return (_flags & Flag_Power_source) != 0;
    }

    const bool is_Receiver_on_when_idle() const {
        return (_flags & Flag_Receiver_on_when_idle) != 0;
    }

    const bool is_Security_capability() const {
        return (_flags & Flag_Security_capability) != 0;
    }

    const bool is_Allocate_address() const {
        return (_flags & Flag_Allocate_address) != 0;
    }

    uint8_t _flags;
};

}

#endif