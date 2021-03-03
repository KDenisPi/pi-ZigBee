/**
 * net.h
 *
 *  Network information
 *
 *  Created on: Merch 2, 2021
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_NETWORK_H_
#define PI_ZIGBEE_LIB_NETWORK_H_

#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <memory>

#include "ezsp_defs.h"
#include "ezsp_util.h"

namespace zb_ezsp {
namespace net {

class Network {
public:
    Network() {
        clear();
    }

    Network(const zb_ezsp::EmberNetworkParameters& net){

        memcpy(extendedPanId, net.extendedPanId, sizeof(extendedPanId));

        panId = net.panId;
        radioTxPower = net.radioTxPower;
        radioChannel = net.radioChannel;
        joinMethod = net.joinMethod;
        nwkManagerId = net.nwkManagerId;
        nwkUpdateId = net.nwkUpdateId;
        channels = net.channels;
    }

    Network(const zb_ezsp::EmberNetworkParameters* net){
        memcpy(extendedPanId, net->extendedPanId, sizeof(extendedPanId));
        panId = net->panId;
        radioTxPower = net->radioTxPower;
        radioChannel = net->radioChannel;
        joinMethod = net->joinMethod;
        nwkManagerId = net->nwkManagerId;
        nwkUpdateId = net->nwkUpdateId;
        channels = net->channels;
    }

    void clear() {
        memset(extendedPanId, 0x00, sizeof(extendedPanId));
        panId = 0;
        radioTxPower = 8;
        radioChannel = 15;
        joinMethod = EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION;
        nwkManagerId = 0;
        nwkUpdateId = 0;
        channels = 0;
    }

    void set_ext_pan(const ExtendedPanId& e_pan){
        memcpy(extendedPanId, e_pan, sizeof(ExtendedPanId));
    }

    ExtendedPanId extendedPanId; // The network's extended PAN identifier.
    uint16_t panId;              //The network's PAN identifier.
    uint8_t radioTxPower;        // A power setting, in dBm.
    uint8_t radioChannel;        // A radio channel.
    EmberJoinMethod joinMethod;  // The method used to initially join the network.
    EmberNodeId nwkManagerId;    // NWK Manager ID. The ID of the network manager in the current network. This may only be set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method.
    uint8_t nwkUpdateId;         // NWK Update ID. The value of the ZigBee nwkUpdateId known by the stack. This is used to determine the newest instance of the network after a
                                 //PAN ID or channel change. This may only be set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method.
    uint32_t channels;           // NWK channel mask. The list of preferred channels that the NWK manager has told this device to use when searching for the network. This may only be
                                 //set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Network parameters: PAN:%04X Pwr:%02X CH:%02X Join Method:%02x MgrID:%02X UpdID:%02X Chs:%08X",
        panId,
        radioTxPower,
        radioChannel,
        joinMethod,
        nwkManagerId,
        nwkUpdateId,
        channels
        );
        return std::string(buff) + " ExPan:" + Conv::to_string(extendedPanId);
    }

};

} //namespace net
} //namespace zb_ezsp

#endif
