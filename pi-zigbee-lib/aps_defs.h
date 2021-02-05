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
#include <string.h>

#include "logger.h"
#include "ezsp_defs.h"

namespace zb_aps {

/**
 * APS Commands (4.4.1.3 Security Processing of APS Commands)
 */
using aps_cmd = enum APS_Command : uint8_t {
    Unknown         = 0x00,
    Transport_Key   = 0x05,
    Update_Device   = 0x06,
    Remove_Device   = 0x07,
    Request_Key     = 0x08,
    Switch_Key      = 0x09,
    Tunnel_Data     = 0x0E,
    Verify_Key      = 0x0F,
    Confirm_Key     = 0x10
};


/**
 * StandardKeyType Parameter of the Transport-Key, Verify-Key, and Confirm-Key Primitives
 */
enum StandardKeyType : uint8_t {
    Reserved_0              = 0x00,     // Reserved
    Standard_network_key    = 0x01,     // Indicates that the key is a network key to be used in standard security mode
    Reserved_2              = 0x02,     // Reserved
    Application_link_key    = 0x03,     // Indicates the key is a link key used as a basis of security between two devices.
    Trust_Center_link_key   = 0x04,     // Indicates that the key is a link key used as a basis for security between the Trust Center and another device.
    Reserved_5              = 0x05      // 0x05 – 0xFF Reserved
};

/**
 * APS payload
 */
class ApsPayload {
public:
    ApsPayload() : _len(0), _cmd(aps_cmd::Unknown) {}
    virtual ~ApsPayload() {}

    const uint8_t put(uint8_t* buff) const{
        memcpy(buff, _buff, _len);
        return _len;
    }

    const uint8_t len() const {
        return _len;
    }

    const uint8_t* buff() const {
        return _buff;
    }

    /**
     * Table 4.11 TransportKeyData Parameter for a Network Key
     *
     * KeySeqNumber Octet               0x00-0xFF       A sequence number assigned to a network key by the Trust Center and used to distinguish network keys for
     *                                                  purposes of key updates and incoming frame security operations.
     * NetworkKey   Set of 16 octets                    The network key.
     * UseParent    Boolean             TRUE | FALSE    This parameter indicates if the destination device’s parent shall be used to forward the key to the destination device:
     *                                                  TRUE = Use parent, FALSE = Do not use parent
     *  ParentAddress Device address Any valid 64-bit address   If the UseParent is TRUE, then ParentAddress parameter shall contain the extended 64-bit address of
     *                                                          the destination device’s parent device; otherwise, this parameter is not used and need not be set.
     *
     */
    void TransportKey(const zb_ezsp::EmberEUI64& destAddress, zb_ezsp::EmberKeyStruct& key_info){
        _len = 0;
        _buff[_len++] = aps_cmd::Transport_Key;

        memcpy(&_buff[_len], destAddress, sizeof(zb_ezsp::EmberEUI64));
        _len += sizeof(zb_ezsp::EmberEUI64);

        _buff[_len++] = StandardKeyType::Standard_network_key;
        _buff[_len++] = key_info.sequenceNumber;

        memcpy(&_buff[_len], key_info.key, sizeof(zb_ezsp::EmberKeyData));
        _len += sizeof(zb_ezsp::EmberKeyData);

        if(key_info.is_parent_empty())
            _buff[_len++] = 0x00; //Do not use Parent
        else{
            _buff[_len++] = 0x01; //Do not use Parent
            memcpy(&_buff[_len], key_info.partnerEUI64, sizeof(zb_ezsp::EmberEUI64));
            _len += sizeof(zb_ezsp::EmberEUI64);
        }
    }

    aps_cmd _cmd;
    uint8_t _buff[100];
    uint8_t _len;
};

} //namespace zb_aps
#endif