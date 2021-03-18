/**
 * ezsp_itf.cpp
 *
 *  EZSP Interface functions.
 *
 *  Created on: May 27, 2020
 *      Author: Denis Kudia
 */

#include "ezsp.h"
//#include "ezsp_zcl.h"
namespace zb_ezsp {

/**
 * Ping device
 */
void Ezsp::echo() {
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::data_array ech;
    ech.dataLength = 10;
    for(int i=0; i<ech.dataLength; i++){
        ech.data[i] = '0' + 1;
    }

    add2output<zb_ezsp::data_array>(zb_ezsp::EId::ID_Echo, ech);
}

/**
 * Get node ID
 */
void Ezsp::get_nodeId_Eui64() {
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getEui64, no_prm);
}

void Ezsp::get_nodeId() {
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getNodeId, no_prm);
}

/**
 * This function will start a scan
 * ch2scan - number channels to scan (15 channels, from 11 to 26). OxFF - scan all
 * channels - array of channels have to be scan. Not used if ch2scan = 0xFF
 * duration - Sets the exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods.
 */
void Ezsp::startScan(const zb_ezsp::EzspNetworkScanType scanType, const uint8_t ch2scan, const uint8_t* channels, const uint8_t duration) {
    zb_ezsp::start_scan scan;
    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Type: " + std::to_string((uint16_t)scanType) + " Channels: " + std::to_string((uint16_t)ch2scan) + " Duration: " + std::to_string((uint16_t)duration));

    if(ch2scan == 0xFF){
        scan.channelMask = 0x07FFF800;
    }
    else{
        for(int i =0; i<ch2scan; i++){
            scan.channelMask |= (1<<channels[i]);
            scan.channelMask &= 0x07FFF800; //clear incorrect channels
        }
    }

    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Channel Mask: " + std::to_string(scan.channelMask));

    scan.scanType = scanType;
    scan.duration = duration;
    add2output<zb_ezsp::start_scan>(zb_ezsp::EId::ID_startScan, scan);
}

void Ezsp::stopScan(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_stopScan, no_prm);
}

/**
 * Allows the NCP to respond with a pending callback
 */
void Ezsp::allowCallback(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_callback, no_prm);
}

/**
 * Value get/set
 */
void Ezsp::getValue(const EzspValueId id){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " id:" + std::to_string(id));

    zb_ezsp::value_get_req v_id;
    v_id.valueId = id;
    add2output<zb_ezsp::value_get_req>(zb_ezsp::EId::ID_getValue, v_id);
}

void Ezsp::getValueTokenStackNodeData(){
    getValue(EFrame_EzspValueId::EZSP_VALUE_TOKEN_STACK_NODE_DATA);
}

/**
 * Configuration value get/set
 */
void Ezsp::getCinfigurationValue(const uint8_t id){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::configid_get_req v_cfg;
    v_cfg.configId = (EzspConfigId)id;
    add2output<zb_ezsp::configid_get_req>(zb_ezsp::EId::ID_getConfigurationValue, v_cfg);
}

void Ezsp::setCinfigurationValue(const uint8_t id, const uint16_t value){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::configid_set_req v_cfg;
    v_cfg.configId = (EzspConfigId)id;
    v_cfg.value = value;
    add2output<zb_ezsp::configid_set_req>(zb_ezsp::EId::ID_setConfigurationValue, v_cfg);
}


/**
 * Init network
 */
void Ezsp::networkInitExt(const EmberNetworkInitBitmask bitmask/* = EmberNetworkInitBitmask::EMBER_NETWORK_INIT_NO_OPTIONS*/) {
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::EmberNetworkInitStruct net;
    net.bitmask = bitmask;
    add2output<zb_ezsp::EmberNetworkInitStruct>(zb_ezsp::EId::ID_networkInitExtended, net);
}

void Ezsp::networkInit(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_networkInit, no_prm);
}

void Ezsp::formNetwork(){

    //Use net 0
    EmberNetworkParameters netPrm;
    auto net = _networks->at(0).get();

    Conv::copy_ext_pan(net->extendedPanId, netPrm.extendedPanId);
    netPrm.radioTxPower = net->radioTxPower;  //dBm
    netPrm.radioChannel = net->radioChannel;
    netPrm.joinMethod = net->joinMethod;
    netPrm.nwkManagerId = net->nwkManagerId;
    netPrm.nwkUpdateId = net->nwkUpdateId;
    netPrm.channels = net->channels;

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Net: " + netPrm.to_string());

    add2output<zb_ezsp::EmberNetworkParameters>(zb_ezsp::EId::ID_formNetwork, netPrm);
}

/**
 * Causes the stack to leave the current network. This generates a stackStatusHandler callback to indicate that the network
 * is down. The radio will not be used until after sending a formNetwork or joinNetwork command.
 */
void Ezsp::leaveNetwork(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_leaveNetwork, no_prm);
}

/**
 * A value of 0x00 disables joining. A value of 0xFF enables joining. Any other value enables joining for that number of seconds.
 */
void Ezsp::permitJoining(const uint8_t duration/*=0xFF*/){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::permitJoining permit;
    permit.duration = duration;
    add2output<zb_ezsp::permitJoining>(zb_ezsp::EId::ID_permitJoining, permit);
}

void Ezsp::getNetworkParameters(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getNetworkParameters, no_prm);
}

/**
 * Security
 */

/*
* Set Initial Security State
* Will set:
*   EMBER_GLOBAL_LINK_KEY, EMBER_HAVE_NETWORK_KEY
*
*/
void Ezsp::setInitialSecurityState(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    EmberInitialSecurityState secSt;

    secSt.clear();
    secSt.bitmask = EmberSecurityBitmaskMode::EMBER_HAVE_PRECONFIGURED_KEY | EmberSecurityBitmaskMode::EMBER_REQUIRE_ENCRYPTED_KEY;
    memcpy(secSt.preconfiguredKey, DefaultGlobalTrustCenterLinkKey, sizeof(EmberKeyData));

    if( is_coordinator()){
        secSt.bitmask |= (EmberSecurityBitmaskMode::EMBER_TRUST_CENTER_GLOBAL_LINK_KEY | EmberSecurityBitmaskMode::EMBER_HAVE_NETWORK_KEY);

        //Network key
        if( _key_network.is_empty()){
            //temporary let's use some hardcode
            //TODO: Add Key generator
            EmberKeyData defKey = {0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0D};

            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " No key generate new one");
            memcpy(_key_network.key, defKey, sizeof(EmberKeyData));
        }

        memcpy(secSt.networkKey, _key_network.key, sizeof(EmberKeyData));
        secSt.networkKeySequenceNumber = 0;
    }

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + secSt.to_string());

    add2output<zb_ezsp::EmberInitialSecurityState>(zb_ezsp::EId::ID_setInitialSecurityState, secSt);
}

void Ezsp::getCurrentSecurityState(){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getCurrentSecurityState, no_prm);
}

/**
 * Messaging
 */
void Ezsp::sendUnicast(const EmberNodeId node_id,
                    const zb_aps::ApsPayload& data,
                    const uint8_t cluster,
                    const uint8_t profile,
                    const uint8_t srcEp,
                    const uint8_t dstEp,
                    const uint8_t seq,
                    const uint8_t tag){

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ID: " + Conv::to_string(node_id));

    zb_ezsp::sendUnicast send_uni;

    /**
     * Try to send packet to the first connected device
     */
    send_uni.apsFrame.profileId = profile; //0x0104;   //Home automation
    send_uni.apsFrame.clusterId = cluster; //0x0402;
    send_uni.apsFrame.sourceEndpoint = srcEp;
    send_uni.apsFrame.destinationEndpoint = dstEp;
    send_uni.apsFrame.options = EmberApsOption::EMBER_APS_OPTION_RETRY | EmberApsOption::EMBER_APS_OPTION_DESTINATION_EUI64; //EmberApsOption::EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY |
    send_uni.apsFrame.groupId = 0;
    send_uni.apsFrame.sequence = seq;

    send_uni.type = EmberOutgoingMessageType::EMBER_OUTGOING_DIRECT;
    send_uni.indexOrDestination = node_id;
    send_uni.messageTag = tag;
    send_uni.messageLength = data.put(send_uni.messageContents);

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + send_uni.to_string());

    add2output<zb_ezsp::sendUnicast>(zb_ezsp::EId::ID_sendUnicast, send_uni);
}

/**
 *
 */
/*
void Ezsp::sendApsTransportKey(const childs::child_info& child){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ID: " + Conv::to_string(child->childEui64));

    zb_aps::ApsPayload data;

    if(child->is_in_progress()){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ID: " + Conv::to_string(child->childEui64) + " Do not send. Is in progress");
        return;
    }
    child->in_progress(true);
    uint8_t seq = child->seq();
    data.TransportKey(child->childEui64, _key_network);
    sendUnicast(child->childId, data, 0, 0, 0, 0, seq, 0x05);
}
*/

/**
 *
 */
void Ezsp::sendZcl(const EmberEUI64& childEui64){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    /**
     * Try to send packet to the first connected device
     */
    const childs::child_info cld = _childs->get_child_obj(childEui64);
    uint8_t tag = 1;
    if(cld){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + cld->to_string());

        zb_ezsp::sendUnicast send_uni;
        send_uni.type = EmberOutgoingMessageType::EMBER_OUTGOING_DIRECT;
        send_uni.indexOrDestination = cld->nwkAddr;
        send_uni.messageTag = tag++;
        send_uni.messageLength = 0;

        send_uni.apsFrame.profileId = 0x0104;   //Home automation
        send_uni.apsFrame.clusterId = 0x0402;
        send_uni.apsFrame.sourceEndpoint = 0x01;
        send_uni.apsFrame.destinationEndpoint = 0x01;
        send_uni.apsFrame.options = EmberApsOption::EMBER_APS_OPTION_NONE;
        send_uni.apsFrame.groupId = 0;
        send_uni.apsFrame.sequence = 0x01;

        /**
         * ZCL
         */
        zb_ezsp::zcl::ZclFrame fzcl(zb_ezsp::zcl::GeneralCmd::DiscoverAttr);
        fzcl.Serv2Cln();
        send_uni.messageLength = fzcl.put(send_uni.messageContents, sizeof(send_uni.messageContents));

        //Debug only
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + send_uni.to_string());
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ZCL " + Conv::print_buff(send_uni.messageContents, send_uni.messageLength));

        add2output<zb_ezsp::sendUnicast>(zb_ezsp::EId::ID_sendUnicast, send_uni);
    }
}

/**
 * Get Route Table Entry
 *
 * @params index The index of the route table entry of interest.
 */
void Ezsp::getRouteTableEntry(const uint8_t& index){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    struct get_by_index info;
    info.index = index;
    add2output<zb_ezsp::get_by_index>(zb_ezsp::EId::ID_getRouteTableEntry, info);
}

/**
 *
 */
void Ezsp::setExtendedTimeout(const EmberEUI64& remoteNodeId, bool extTimeout /*= true*/){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    auto child = _childs->get_child_obj(remoteNodeId);
    if(child){
        struct setExtendedTimeout extTm;
        memcpy(extTm.remoteEui64 , child->ieeeAddr, sizeof(extTm.remoteEui64));
        extTm.extendedTimeout = extTimeout;

        add2output<zb_ezsp::setExtendedTimeout>(zb_ezsp::EId::ID_setExtendedTimeout, extTm);
    }
}

void Ezsp::getExtendedTimeout(const EmberEUI64&  remoteNodeId){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    auto child = _childs->get_child_obj(remoteNodeId);
    if(child){
        struct getExtendedTimeout extTm;
        memcpy(extTm.remoteEui64 , child->ieeeAddr, sizeof(extTm.remoteEui64));

        add2output<zb_ezsp::getExtendedTimeout>(zb_ezsp::EId::ID_getExtendedTimeout, extTm);
    }
}


/**
 * Binding
 */
void Ezsp::setBinding(const EmberEUI64& childEui64){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

    zb_ezsp::setBinding_req set_bnd;
    uint8_t bnd_idx = 0;

    const childs::child_info cld = _childs->get_child_obj(childEui64);
    if(cld){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + cld->to_string());


        set_bnd.index = bnd_idx++;
        set_bnd.value.type = EmberBindingType::EMBER_UNICAST_BINDING;
        set_bnd.value.local = 0x01; //0x00;
        set_bnd.value.clusterId = 0x0402;
        set_bnd.value.remote = 0x01; //0x00;
        memcpy(set_bnd.value.identifier, cld->ieeeAddr, sizeof(EmberEUI64));
        set_bnd.value.networkIndex = 0x00; //TODO: get network index

        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Bind: " + set_bnd.to_string());

        add2output<zb_ezsp::setBinding_req>(zb_ezsp::EId::ID_setBinding, set_bnd);
    }
    else {
        logger::log(logger::LLOG::ERROR, "ezsp", std::string(__func__) + " No child");
        return;
    }
}

}//namespace zb_ezsp
