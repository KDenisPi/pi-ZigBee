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

/**
 * Command without any parameters
 */
using no_params = struct ezsp_no_params {

};

/**
 * Response with Ember Status only
 */
using ember_status = struct ezsp_EmberStatus {
    EmberStatus status;

    const std::string to_string() const {
        char buff[20];
        std::sprintf(buff, "Status: 0x%02X", status);
        return std::string(buff);
    }
};

using ezsp_status = struct ezsp_EzspStatus {
    EzspStatus status;

    const std::string to_string() const {
        char buff[20];
        std::sprintf(buff, "Status: 0x%02X", status);
        return std::string(buff);
    }
};


/**
 * Start scan & related structures
 */
using start_scan = struct ezsp_StartScan {
    EzspNetworkScanType scanType;
    uint32_t channelMask;
    uint8_t duration;
};

/**
 * Reports the result of an energy scan for a single channel. The scan is not complete until the scanCompleteHandler callback is called.
 */
using energyScanResultHandler = struct ezsp_energyScanResultHandler {
    uint8_t channel;        //The 802.15.4 channel number that was scanned.
    int8_t maxRssiValue;    //The maximum RSSI value found on the channel.

    const std::string to_string() const {
        return std::string("Scan result Channel: ") + std::to_string((uint16_t)channel) + " Max RSSI: " + std::to_string((int16_t)maxRssiValue);
    }
};

/**
 * The parameters of a ZigBee network.
 */
using  EmberZigbeeNetwork = struct ezsp_EmberZigbeeNetwork {
    uint8_t channel;            // The 802.15.4 channel associated with the network.
    uint16_t panId;             // The network's PAN identifier.
    uint8_t extendedPanId[8];   // The network's extended PAN identifier.
    EZSP_Bool allowingJoin;     // Whether the network is allowing MAC associations.
    uint8_t stackProfile;       // The Stack Profile associated with the network.
    uint8_t nwkUpdateId;        // The instance of the Network.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Zigbee Network: CH:%d PAN:%d  ExPan:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X Allow Join:%d StProfile:%02X NetInstance:%02X",
        channel,
        panId,
        extendedPanId[0],
        extendedPanId[1],
        extendedPanId[2],
        extendedPanId[3],
        extendedPanId[4],
        extendedPanId[5],
        extendedPanId[6],
        extendedPanId[7],
        allowingJoin,
        stackProfile,
        nwkUpdateId
        );
        return std::string(buff);
    }
};

/**
 * Reports that a network was found as a result of a prior call to startScan. Gives the network parameters useful for deciding which network to join.
 */
using networkFoundHandler = struct ezsp_networkFoundHandler {
    EmberZigbeeNetwork networkFound;    // The parameters associated with the network found.
    uint8_t lastHopLqi;                 // The link quality from the node that generated this beacon.
    int8_t lastHopRssi;                 // The energy level (in units of dBm) observed during the reception.

    const std::string to_string() const {
        std::string result = " Net Found Handler: lastHopLqi:" + std::to_string((uint16_t)lastHopLqi) + " lastHopRssi:" + std::to_string((int16_t)lastHopRssi) + " \n" + networkFound.to_string();
        return result;
    }
};

using scanCompleteHandler = struct ezsp_scanCompleteHandler {
    uint8_t channel;    // The channel on which the current error occurred. Undefined for the case of EMBER_SUCCESS.
    EmberStatus status; // The error condition that occurred on the current channel. Value will be EMBER_SUCCESS when the scan has completed.

    const std::string to_string() const {
        char buff[40];
        if(EmberStatus::EMBER_SUCCESS == status)
            std::sprintf(buff, "Status: 0x%02X", status);
        else
        {
            std::sprintf(buff, "Status: 0x%02X Channel: %d", status, channel);
        }
        return std::string(buff);
    }
};

using value_set = struct ezsp_ncp_value_set {
    EzspValueId valueId;    // EZSP_SUCCESS if the value was read successfully, EZSP_ERROR_INVALID_ID if the NCP does not recognize valueId.
    uint8_t valueLength;    // The length of the value parameter in bytes.
    uint8_t value[100];     // The value.
};

using value_get_req = struct ezsp_ncp_value_get_req {
    EzspValueId valueId;
};

using value_get_resp = struct ezsp_ncp_value_get_resp {
    EzspStatus status;      // EZSP_SUCCESS if the value was read successfully, EZSP_ERROR_INVALID_ID if the NCP does not recognize valueId.
    uint8_t valueLength;    // The length of the value parameter in bytes.
    uint8_t value[100];     // The value.

    const std::string to_string() const {
        char buff[40];
        std::sprintf(buff, "Status: 0x%02X Length: %d", status, valueLength);
        return std::string(buff);
    }
};

/**
 * Config id
 */
using configid_get_req = struct ezsp_configid_get_req {
    EzspConfigId configId;
};

using configid_get_resp = struct ezsp_configid_get_resp {
    EzspStatus status;      // EZSP_SUCCESS if the value was read successfully, EZSP_ERROR_INVALID_ID if the NCP does not recognize valueId.
    uint16_t value;

    const std::string to_string() const {
        char buff[40];
        std::sprintf(buff, "Status: 0x%02X Value: %d", status, value);
        return std::string(buff);
    }
};

using configid_set_req = struct ezsp_configid_set_req {
    EzspConfigId configId;
    uint16_t value;
};

using networkState = struct ezsp_networkState {
    EmberNetworkStatus status;

    const std::string to_string() const {
        char buff[40];
        std::sprintf(buff, "Status: 0x%02X", status);
        return std::string(buff);
    }
};

struct EmberNetworkInitStruct {
    EmberNetworkInitBitmask bitmask;
};

struct EmberNetworkParameters {
    uint8_t extendedPanId[8];   // The network's extended PAN identifier.
    uint16_t panId;             //The network's PAN identifier.
    uint8_t radioTxPower;       // A power setting, in dBm.
    uint8_t radioChannel;       // A radio channel.
    EmberJoinMethod joinMethod; // The method used to initially join the network.
    EmberNodeId nwkManagerId;   // NWK Manager ID. The ID of the network manager in the current network. This may only be set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method.
    uint8_t nwkUpdateId;        // NWK Update ID. The value of the ZigBee nwkUpdateId known by the stack. This is used to determine the newest instance of the network after a
                                //PAN ID or channel change. This may only be set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method.
    uint32_t channels;          // NWK channel mask. The list of preferred channels that the NWK manager has told this device to use when searching for the network. This may only be
                                //set at joining when using EMBER_USE_NWK_COMMISSIONING as the join method

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Network parameters: PAN:%d ExPan:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X Pwr:%02X CH:%02X Join Method:%02x MgrID:%02X UpdID:%02X Chs:%08X",
        panId,
        extendedPanId[0],
        extendedPanId[1],
        extendedPanId[2],
        extendedPanId[3],
        extendedPanId[4],
        extendedPanId[5],
        extendedPanId[6],
        extendedPanId[7],
        radioTxPower,
        radioChannel,
        joinMethod,
        nwkManagerId,
        nwkUpdateId,
        channels
        );
        return std::string(buff);
    }

};

using getNetworkParameters_resp = struct ezsp_getNetworkParameters {
    EmberStatus status;                 // An EmberStatus value indicating success or the reason for failure.
    EmberNodeType nodeType;             // An EmberNodeType value indicating the current node type.
    EmberNetworkParameters parameters;  // The current network parameters.

    const std::string to_string() const {
        char buff[40];
        std::sprintf(buff, "Status: 0x%02X Node Type: %d", status, nodeType);
        return std::string(buff) + "\n" + parameters.to_string();
    }
};

}
#endif