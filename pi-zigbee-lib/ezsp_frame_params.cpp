/**
 * ezsp_frame_params.cpp
 *
 *  EZSP Frames. Parameters
 *
 *  Created on: May 6, 2020
 *      Author: Denis Kudia
 */

#include <stddef.h>
#include <assert.h>

#include "ezsp_frame_params.h"
#include "ezsp_frame.h"

namespace zb_ezsp {

size_t EFrame::get_param(EmberEUI64& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param, sizeof(param), sizeof(param));
    return pos;
}
size_t EFrame::get_param(Eui64& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.eui64, sizeof(param.eui64), sizeof(param.eui64));
    //pos = get_param(param, buff, pos);
    return pos;
}

size_t EFrame::get_param(NodeId& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.nodeId);
    return pos;
}

size_t EFrame::get_param(uint8t_value& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.value);
    return pos;
}

/**
 * Version
 */
size_t EFrame::put_param(const ver_req& param, uint8_t* buff, size_t pos){
    return Conv::put(buff, pos, param._ver);
}

size_t EFrame::get_param(ver_resp& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param._ver);
    pos = Conv::get(buff, pos, param._stackType);
    pos = Conv::get(buff, pos, param._stackVersion);
    return pos;
}

/**
 * Echo
 */
size_t EFrame::put_param(const zb_ezsp::data_array& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.dataLength);
    pos = Conv::put(buff, pos, param.data, param.dataLength, free_len(pos));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::data_array& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.dataLength);
    pos = Conv::get(buff, pos, param.data, param.dataLength, sizeof(param.data));
    return pos;
}

/**
 * Request and response without parameters
 */
size_t EFrame::put_param(const zb_ezsp::ezsp_no_params& param, uint8_t* buff, size_t pos){
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::uint8t_value& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.value);
    return pos;
}


size_t EFrame::get_param(zb_ezsp::ezsp_no_params& param, const uint8_t* buff, size_t& pos){
    return pos;
}

/**
 * Ember status only
 */
size_t EFrame::get_param(zb_ezsp::ezsp_EmberStatus& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    return pos;
}

/**
 * EZSP status only
 */
size_t EFrame::get_param(zb_ezsp::ezsp_EzspStatus& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EzspStatus>(buff, pos, param.status);
    return pos;
}

/**
 * Start scan and related calls
 */
size_t EFrame::put_param(const zb_ezsp::start_scan& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.scanType);
    pos = Conv::put(buff, pos, param.channelMask);
    pos = Conv::put(buff, pos, param.duration);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::energyScanResultHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.maxRssiValue);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::EmberZigbeeNetwork& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.panId);
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.extendedPanId, sizeof(param.extendedPanId), sizeof(param.extendedPanId));
    pos = Conv::get(buff, pos, param.stackProfile);
    pos = Conv::get(buff, pos, param.nwkUpdateId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::networkFoundHandler& param, const uint8_t* buff, size_t& pos){
    pos = get_param(param.networkFound, buff, pos);
    pos = Conv::get(buff, pos, param.lastHopLqi);
    pos = Conv::get(buff, pos, param.lastHopRssi);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::scanCompleteHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::networkState& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberNetworkStatus>(buff, pos, param.status);
    return pos;
}


/**
 * Value
 */
size_t EFrame::put_param(const zb_ezsp::value_set& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.valueId);
    pos = Conv::put(buff, pos, param.valueLength);
    pos = Conv::put(buff, pos, param.value, param.valueLength, free_len(pos));
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::value_get_req& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.valueId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::value_get_resp& param, const uint8_t* buff, size_t& pos){
    param.valueLength = 0;
    pos = Conv::get_byte<EzspStatus>(buff, pos, param.status);
    if(param.status == EzspStatus::EZSP_SUCCESS){
        pos = Conv::get(buff, pos, param.valueLength);
        pos = Conv::get(buff, pos, param.value, param.valueLength, sizeof(param.value));
    }
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::EmberNetworkInitStruct& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint16_t)param.bitmask);
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::EmberNetworkParameters& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.extendedPanId, sizeof(param.extendedPanId), sizeof(param.extendedPanId));
    pos = Conv::put(buff, pos, param.panId);
    pos = Conv::put(buff, pos, param.radioTxPower);
    pos = Conv::put(buff, pos, param.radioChannel);
    pos = Conv::put(buff, pos, (uint8_t)param.joinMethod);
    pos = Conv::put(buff, pos, param.nwkManagerId);
    pos = Conv::put(buff, pos, param.nwkUpdateId);
    pos = Conv::put(buff, pos, param.channels);
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::permitJoining& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.duration);
    return pos;
}


size_t EFrame::get_param(zb_ezsp::EmberNetworkParameters& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.extendedPanId, sizeof(param.extendedPanId), sizeof(param.extendedPanId));
    pos = Conv::get(buff, pos, param.panId);
    pos = Conv::get(buff, pos, param.radioTxPower);
    pos = Conv::get(buff, pos, param.radioChannel);
    pos = Conv::get_byte<EmberJoinMethod>(buff, pos, param.joinMethod);
    pos = Conv::get(buff, pos, param.nwkManagerId);
    pos = Conv::get(buff, pos, param.nwkUpdateId);
    pos = Conv::get(buff, pos, param.channels);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getNetworkParameters_resp& param, const uint8_t* buff, size_t pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = Conv::get_byte<EmberNodeType>(buff, pos, param.nodeType);
    pos = get_param(param.parameters, buff, pos);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::childJoinHandler& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.index);
    pos = Conv::get(buff, pos, param.joining);
    pos = Conv::get(buff, pos, param.childId);
    pos = Conv::get(buff, pos, param.childEui64, sizeof(param.childEui64), sizeof(param.childEui64));
    pos = Conv::get_byte<EmberNodeType>(buff, pos, param.childType);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::trustCenterJoinHandler& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.newNodeId);
    pos = Conv::get(buff, pos, param.newNodeEui64, sizeof(param.newNodeEui64), sizeof(param.newNodeEui64));
    pos = Conv::get_byte<EmberDeviceUpdate>(buff, pos, param.status);
    pos = Conv::get_byte<EmberJoinDecision>(buff, pos, param.policyDecision);
    pos = Conv::get(buff, pos, param.parentOfNewNodeId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getParentChildParameters& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.childCount);
    pos = Conv::get(buff, pos, param.parentEui64, sizeof(param.parentEui64), sizeof(param.parentEui64));
    pos = Conv::get(buff, pos, param.parentNodeId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::stackTokenChangedHandler& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.tokenAddress);
    return pos;
}


/**
 * Configuration parameters
 */
size_t EFrame::put_param(const zb_ezsp::configid_set_req& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.configId);
    pos = Conv::put(buff, pos, param.value);
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::ezsp_configid_get_req& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.configId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::ezsp_configid_get_resp& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EzspStatus>(buff, pos, param.status);
    pos = Conv::get(buff, pos, param.value);
    return pos;
}

/**
 * Security
 */
size_t EFrame::put_param(const zb_ezsp::EmberInitialSecurityState& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.bitmask);
    pos = Conv::put(buff, pos, param.preconfiguredKey, sizeof(param.preconfiguredKey), sizeof(param.preconfiguredKey));
    pos = Conv::put(buff, pos, param.networkKey, sizeof(param.networkKey), sizeof(param.networkKey));
    pos = Conv::put(buff, pos, param.networkKeySequenceNumber);
    pos = Conv::put(buff, pos, param.preconfiguredTrustCenterEui64, sizeof(param.preconfiguredTrustCenterEui64), sizeof(param.preconfiguredTrustCenterEui64));
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::EmberCurrentSecurityState& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.bitmask);
    pos = Conv::put(buff, pos, param.trustCenterLongAddress, sizeof(param.trustCenterLongAddress), sizeof(param.trustCenterLongAddress));
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::BecomeTrustCenter& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.key, sizeof(param.key), sizeof(param.key));
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::unicastNwkKeyUpdate& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.destShort);
    pos = Conv::put(buff, pos, param.destLong);
    pos = Conv::put(buff, pos, param.key, sizeof(param.key), sizeof(param.key));
    return pos;
}


size_t EFrame::get_param(zb_ezsp::EmberCurrentSecurityState& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_word<EmberSecurityBitmask>(buff, pos, param.bitmask);
    pos = Conv::get(buff, pos, param.trustCenterLongAddress, sizeof(param.trustCenterLongAddress), sizeof(param.trustCenterLongAddress));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getCurrentSecurityState& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = get_param(param.state, buff, pos);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::EmberKeyStruct& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_word<EmberKeyStructBitmask>(buff, pos, param.bitmask);
    pos = Conv::get_byte<EmberKeyType>(buff, pos, param.type);
    pos = Conv::get(buff, pos, param.key, sizeof(param.key), sizeof(param.key));
    pos = Conv::get(buff, pos, param.outgoingFrameCounter);
    pos = Conv::get(buff, pos, param.incomingFrameCounter);
    pos = Conv::get(buff, pos, param.sequenceNumber);
    pos = Conv::get(buff, pos, param.partnerEUI64, sizeof(param.partnerEUI64), sizeof(param.partnerEUI64));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getKey& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = get_param(param.keyStruct, buff, pos);
    return pos;
}


/**
 * APS Frame
 */
size_t EFrame::put_param(const zb_ezsp::EmberApsFrame& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.profileId);
    pos = Conv::put(buff, pos, param.clusterId);
    pos = Conv::put(buff, pos, param.sourceEndpoint);
    pos = Conv::put(buff, pos, param.destinationEndpoint);
    pos = Conv::put(buff, pos, (uint16_t)param.options);
    pos = Conv::put(buff, pos, param.groupId);
    pos = Conv::put(buff, pos, param.sequence);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::EmberApsFrame& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.profileId);
    pos = Conv::get(buff, pos, param.clusterId);
    pos = Conv::get(buff, pos, param.sourceEndpoint);
    pos = Conv::get(buff, pos, param.destinationEndpoint);
    pos = Conv::get_word<EmberApsOption>(buff, pos, param.options);
    pos = Conv::get(buff, pos, param.groupId);
    pos = Conv::get(buff, pos, param.sequence);
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::sendUnicast& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.type);
    pos = Conv::put(buff, pos, param.indexOrDestination);
    pos = put_param(param.apsFrame, buff, pos);
    pos = Conv::put(buff, pos, param.messageTag);
    pos = Conv::put(buff, pos, param.messageLength);
    pos = Conv::put(buff, pos, param.messageContents, param.messageLength, sizeof(param.messageContents));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::sendUnicast_resp& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = Conv::get(buff, pos, param.sequence);
    return pos;
}


size_t EFrame::get_param(zb_ezsp::incomingMessageHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberIncomingMessageType>(buff, pos, param.type);
    pos = get_param(param.apsFrame, buff, pos);
    pos = Conv::get(buff, pos, param.lastHopLqi);
    pos = Conv::get(buff, pos, param.lastHopRssi);
    pos = Conv::get(buff, pos, param.sender);
    pos = Conv::get(buff, pos, param.bindingIndex);
    pos = Conv::get(buff, pos, param.addressIndex);
    pos = Conv::get(buff, pos, param.messageLength);
    pos = Conv::get(buff, pos, param.messageContents, param.messageLength, sizeof(param.messageContents));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::messageSentHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberOutgoingMessageType>(buff, pos, param.type);
    pos = Conv::get(buff, pos, param.indexOrDestination);
    pos = get_param(param.apsFrame, buff, pos);
    pos = Conv::get(buff, pos, param.messageTag);
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = Conv::get(buff, pos, param.messageLength);
    pos = Conv::get(buff, pos, param.messageContents, param.messageLength, sizeof(param.messageContents));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::incomingRouteErrorHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = Conv::get(buff, pos, param.target);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getChildData& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = Conv::get(buff, pos, param.childId);
    pos = Conv::get(buff, pos, param.childEui64, sizeof(param.childEui64), sizeof(param.childEui64));
    pos = Conv::get_byte<EmberNodeType>(buff, pos, param.childType);
    return pos;
}


size_t EFrame::put_param(const zb_ezsp::setExtendedTimeout& param, uint8_t* buff, size_t pos){
    //pos = Conv::put(buff, pos, param.remoteEui64);
    pos = Conv::put(buff, pos, param.remoteEui64, sizeof(param.remoteEui64), sizeof(param.remoteEui64));
    pos = Conv::put(buff, pos, (uint8_t)param.extendedTimeout);
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::getExtendedTimeout& param, uint8_t* buff, size_t pos){
    //pos = Conv::put(buff, pos, param.remoteEui64);
    pos = Conv::put(buff, pos, param.remoteEui64, sizeof(param.remoteEui64), sizeof(param.remoteEui64));
    return pos;
}


/**
 * Get data by Index
 */
size_t EFrame::put_param(const zb_ezsp::get_by_index& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.index);
    return pos;
}

/**
 * Binding
 */
size_t EFrame::put_param(const zb_ezsp::EmberBindingTableEntry& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.type);
    pos = Conv::put(buff, pos, param.local);
    pos = Conv::put(buff, pos, param.clusterId);
    pos = Conv::put(buff, pos, param.remote);
    pos = Conv::put(buff, pos, param.identifier, sizeof(param.identifier), sizeof(param.identifier));
    pos = Conv::put(buff, pos, param.networkIndex);
    return pos;

}

size_t EFrame::get_param(zb_ezsp::EmberBindingTableEntry& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberBindingType>(buff, pos, param.type);
    pos = Conv::get(buff, pos, param.local);
    pos = Conv::get(buff, pos, param.clusterId);
    pos = Conv::get(buff, pos, param.remote);
    pos = Conv::get(buff, pos, param.identifier, sizeof(param.identifier), sizeof(param.identifier));
    pos = Conv::get(buff, pos, param.networkIndex);
    return pos;

}

size_t EFrame::put_param(const zb_ezsp::setBinding_req& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.index);
    pos = put_param(param.value, buff, pos);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::getBinding& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get_byte<EmberStatus>(buff, pos, param.status);
    pos = get_param(param.value, buff, pos);
    return pos;
}



}//namespace zb_ezsp