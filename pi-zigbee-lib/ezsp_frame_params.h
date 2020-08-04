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
#include <cstring>
#include "ezsp_defs.h"
#include "ezsp_util.h"

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
 * Echo command ID 0x81, and other command view [dataLength, array]
*
 */
using data_array = struct ezsp_data_array { //__attribute__((packed, aligned(1)))
    uint8_t dataLength;        // The length of the data parameter in bytes.
    uint8_t data[120];         // The data to be echoed back.

    const std::string to_string() const {
        std::string result = " Length: " + std::to_string(dataLength) + " Data: " + Conv::print_buff(data, dataLength);
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

using uint8t_value = struct ezsp_8tValue {
    uint value;

    const std::string to_string() const {
        char buff[20];
        std::sprintf(buff, "Value: 0x%02X", value);
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
        std::sprintf(buff, "Zigbee Network: CH:%d PAN:%d Allow Join:%d StProfile:%02X NetInstance:%02X",
        channel,
        panId,
        allowingJoin,
        stackProfile,
        nwkUpdateId
        );
        return std::string(buff)+ " ExPan:" + Conv::Eui64_to_string(extendedPanId);
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

class EmberNetworkParameters {
public:
    EmberNetworkParameters() {
        memset(extendedPanId, 0x00, sizeof(extendedPanId));
        panId = 0;
        radioTxPower = 8;
        radioChannel = 11;
        joinMethod = EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION;
        nwkManagerId = 0;
        nwkUpdateId = 0;
        channels = 0;
    }

    EmberNetworkParameters(const EmberNetworkParameters& net){

        memcpy(extendedPanId, net.extendedPanId, sizeof(extendedPanId));

        panId = net.panId;
        radioTxPower = net.radioTxPower;
        radioChannel = net.radioChannel;
        joinMethod = net.joinMethod;
        nwkManagerId = net.nwkManagerId;
        nwkUpdateId = net.nwkUpdateId;
        channels = net.channels;
    }

    EmberNetworkParameters(const EmberNetworkParameters* net){
        memcpy(extendedPanId, net->extendedPanId, sizeof(extendedPanId));
        panId = net->panId;
        radioTxPower = net->radioTxPower;
        radioChannel = net->radioChannel;
        joinMethod = net->joinMethod;
        nwkManagerId = net->nwkManagerId;
        nwkUpdateId = net->nwkUpdateId;
        channels = net->channels;
    }

    void set_ext_pan(const uint8_t* e_pan){
        memcpy(extendedPanId, e_pan, sizeof(extendedPanId));
    }

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
        std::sprintf(buff, "Network parameters: PAN:%d Pwr:%02X CH:%02X Join Method:%02x MgrID:%02X UpdID:%02X Chs:%08X",
        panId,
        radioTxPower,
        radioChannel,
        joinMethod,
        nwkManagerId,
        nwkUpdateId,
        channels
        );
        return std::string(buff) + " ExPan:" + Conv::Eui64_to_string(extendedPanId);
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

struct permitJoining {
    uint8_t duration;   //A value of 0x00 disables joining. A value of 0xFF enables joining. Any other value enables joining for that number of seconds.
};


struct EmberCurrentSecurityState {
    EmberCurrentSecurityBitmask bitmask;    //A bitmask indicating the security options currently in use by a device joined in the network.
    EmberEUI64 trustCenterLongAddress;      // The IEEE Address of the Trust Center device.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Security state: Bitmask:%04X",
        bitmask
        );
        return std::string(buff) + " Trust center:" + Conv::Eui64_to_string(trustCenterLongAddress);
    }

};

struct getCurrentSecurityState {
    EmberStatus status;                 // An EmberStatus value indicating success or the reason for failure.
    EmberCurrentSecurityState state;

    const std::string to_string() const {
        char buff[40];
        std::sprintf(buff, "Status: 0x%02X", status);
        return std::string(buff) + " " + state.to_string();
    }
};

struct childJoinHandler {
    uint8_t index;              // The index of the child of interest.
    EZSP_Bool joining;          // True if the child is joining. False the child is leaving.
    EmberNodeId childId;        // The node ID of the child.
    EmberEUI64 childEui64;      // The EUI64 of the child.
    EmberNodeType childType;    // The node type of the child.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Child: Index:%d Joining:%d ID:%04X Type:%02X",
        index,
        joining,
        childId,
        childType
        );
        return std::string(buff) + Conv::Eui64_to_string(childEui64);
    }
};

struct Eui64 {
    EmberEUI64 eui64;
};

struct NodeId {
    EmberNodeId nodeId;
};

/**
 * The NCP used the trust center behavior policy to decide whether to allow a new node to join the network. The Host cannot
 *   change the current decision, but it can change the policy for future decisions using the setPolicy command.
 */
struct trustCenterJoinHandler {
    EmberNodeId newNodeId;      //The Node Id of the node whose status changed
    EmberEUI64 newNodeEui64;    // The EUI64 of the node whose status changed.
    EmberDeviceUpdate status;   // The status of the node: Secure Join/Rejoin, Unsecure Join/Rejoin, Device left.
    EmberJoinDecision policyDecision;   // An EmberJoinDecision reflecting the decision made.
    EmberNodeId parentOfNewNodeId;  // The parent of the node whose status has changed.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "trustCenterJoin: ID:%04X Status:%02X p.Decision:%04X Parent:%04X ",
        newNodeId,
        status,
        policyDecision,
        parentOfNewNodeId
        );
        return std::string(buff) + Conv::Eui64_to_string(newNodeEui64);
    }
};

/**
 * ZigBee APS frame parameters.
 */
struct EmberApsFrame {
    uint16_t profileId;         // The application profile ID that describes the format of the message.
    uint16_t clusterId;         // The cluster ID for this message.
    uint8_t sourceEndpoint;     // The source endpoint.
    uint8_t destinationEndpoint;// The destination endpoint.
    EmberApsOption options;     // A bitmask of options.
    uint16_t groupId;           // The group ID for this message, if it is multicast mode.
    uint8_t sequence;           // The sequence number.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " EmberApsFrame profileId:%04X clusterId:%04X Endpoint src:%02X dest:%02X Options:%04X GrID:%04X Seq:%02X",
        profileId,
        clusterId,
        sourceEndpoint,
        destinationEndpoint,
        options,
        groupId,
        sequence
        );
        return std::string(buff);
    }
};

struct sendUnicast {
    EmberOutgoingMessageType type;  // Specifies the outgoing message type. Must be one of EMBER_OUTGOING_DIRECT, EMBER_OUTGOING_VIA_ADDRESS_TABLE, or EMBER_OUTGOING_VIA_BINDING.
    EmberNodeId indexOrDestination; // Depending on the type of addressing used, this is either the EmberNodeId of the destination,
                                    // an index into the address table, or an index into the binding table.
    EmberApsFrame apsFrame;         // The APS frame which is to be added to the message.
    uint8_t messageTag;             // A value chosen by the Host. This value is used in the ezspMessageSentHandler response to refer to this message.
    uint8_t messageLength;          // The length of the messageContents parameter in bytes.
    uint8_t messageContents[100];   // Content of the message.

    const std::string to_string() const {
        char buff[140];
        std::sprintf(buff, " sendUnicast type:%02X Idx:%04X Tag:%02X Len:%02X ",
        type,
        indexOrDestination,
        messageTag,
        messageLength
        );
    }

};

struct sendUnicast_resp {
    EmberStatus status;
    uint8_t sequence;   // The sequence number that will be used when this message is transmitted.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " sendUnicast Status:%02X sequence:%02X",
        status,
        sequence
        );
        return std::string(buff);
    }

};

struct incomingMessageHandler {
    EmberIncomingMessageType type;  //The type of the incoming message. One of the following: EMBER_INCOMING_UNICAST, EMBER_INCOMING_UNICAST_REPLY, EMBER_INCOMING_MULTICAST,
                                    //EMBER_INCOMING_MULTICAST_LOOPBACK, EMBER_INCOMING_BROADCAST, EMBER_INCOMING_BROADCAST_LOOPBACK
    EmberApsFrame apsFrame;         // The APS frame from the incoming message.
    uint8_t lastHopLqi;             // The link quality from the node that last relayed the message.
    int8_t lastHopRssi;             // The energy level (in units of dBm) observed during the reception.
    EmberNodeId sender;             // The sender of the message.
    uint8_t bindingIndex;           // The index of a binding that matches the message or 0xFF if there is no matching binding.
    uint8_t addressIndex;           // The index of the entry in the address table that matches the sender of the message or 0xFF if there is no matching entry.
    uint8_t messageLength;          // The length of the messageContents parameter in bytes.
    uint8_t messageContents[120];   // The incoming message.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " IncomingMessage type:%02X lastHopLqi:%02X lastHopRssi:%02d Sender:%04X bindingIndex:%02X addressIndex:%02X messageLength:%02X",
        type,
        lastHopLqi,
        lastHopRssi,
        sender,
        bindingIndex,
        addressIndex,
        messageLength
        );
        return std::string(buff) + apsFrame.to_string();
    }
};

struct messageSentHandler {
    EmberOutgoingMessageType type;  // The type of message sent.
    uint16_t indexOrDestination;    // The destination to which the message was sent, for direct unicasts, or the address table or binding
                                    //index for other unicasts. The value is unspecified for multicasts and broadcasts.
    EmberApsFrame apsFrame;         // The APS frame for the message.
    uint8_t messageTag;             // The value supplied by the ezspSendMulticast command.
    EmberStatus status;             // An EmberStatus value of EMBER_SUCCESS if an ACK was received from the destination or EMBER_DELIVERY_FAILED if no ACK was received.
    uint8_t messageLength;          // The length of the messageContents parameter in bytes.
    uint8_t messageContents[100];   // The unicast message supplied by the Host. The message contents are only included here if the
                                    //decision for the messageContentsInCallback policy is messageTagAndContentsInCallback.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " messageSent type:%02X indexOrDestination:%04X Tag:%02d Status:%02X messageLength:%02X",
        type,
        indexOrDestination,
        messageTag,
        status,
        messageLength
        );
        return std::string(buff) + apsFrame.to_string();
    }
};

struct incomingRouteErrorHandler {
    EmberStatus status;     //  EMBER_SOURCE_ROUTE_FAILURE EMBER_MANY_TO_ONE_ROUTE_FAILURE.
    EmberNodeId target;     //  The short id of the remote node.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " incomingRouteErrorHandler Status:%02X target:%04X",
        status,
        target
        );
        return std::string(buff);
    }
};

/**
 * Returns information about the children of the local node and the parent of the local node.
 */
struct getParentChildParameters {
    uint8_t childCount;         // The number of children the node currently has.
    EmberEUI64 parentEui64;     // The parent's EUI64. The value is undefined for nodes without parents (coordinators and nodes that are not joined to a network).
    EmberNodeId parentNodeId;   // The parent's node ID. The value is undefined for nodes without parents (coordinators and nodes that are not joined to a network).

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " ParentChildParameters Count:%02X Parent: NodeID:%04X", childCount, parentNodeId);
        return std::string(buff) + Conv::Eui64_to_string(parentEui64);
    }
};

/**
 * Group of requests based on Index number only
 */
struct get_by_index {
    uint8_t index;
};

struct getChildData {
    EmberStatus status;         // EMBER_SUCCESS if there is a child at index. EMBER_NOT_JOINED if there is no child at index.
    EmberNodeId childId;        // The node ID of the child.
    EmberEUI64 childEui64;      // The EUI64 of the child.
    EmberNodeType childType;    // The EmberNodeType value for the child.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " getChildData Status:%02X ID:%04X Type:%02X ",
        status,
        childId,
        childType
        );
        return std::string(buff) + Conv::Eui64_to_string(childEui64);
    }
};

/**
 * An entry in the binding table.
 */
struct EmberBindingTableEntry {
    EmberBindingType type;  // The type of binding.
    uint8_t local;          // The endpoint on the local node.
    uint16_t clusterId;     // A cluster ID that matches one from the local endpoint's simple descriptor. This cluster ID is set by
                            //the provisioning application to indicate which part an endpoint's functionality is bound to this particular remote node and is used to distinguish between
                            //unicast and multicast bindings. Note that a binding can be used to send messages with any cluster ID, not just the one listed in the binding.
    uint8_t remote;         // The endpoint on the remote node (specified by identifier).
    EmberEUI64 identifier;  // A 64-bit identifier. This is either the destination EUI64 (for unicasts) or the 64-bit group address (for multicasts).
    uint8_t networkIndex;   //The index of the network the binding belongs to.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " EmberBindingTableEntry type:%02X local:%02X clusterId:%04X remote:%02X NetIdx:%02X identifier ",
        type,
        local,
        clusterId,
        remote,
        networkIndex
        );
        return std::string(buff) + Conv::Eui64_to_string(identifier);
    }
};

/**
 * Sets an entry in the binding table.
 */
struct setBinding_req {
    uint8_t index;  // The index of a binding table entry. (1-240)
    EmberBindingTableEntry value;   // The contents of the binding entry.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " setBinding Index:%02X ",
            index
        );
        return std::string(buff) + value.to_string();
    }

};

struct getBinding {
    EmberStatus status;             // An EmberStatus value indicating success or the reason for failure.
    EmberBindingTableEntry value;   // The contents of the binding entry.

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " getBinding status:%02X ",
        status
        );
        return std::string(buff) + value.to_string();
    }
};


}
#endif