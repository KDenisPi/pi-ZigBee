/**
 * ezsp_itf.cpp
 *
 *  EZSP Interface functions.
 *
 *  Created on: May 27, 2020
 *      Author: Denis Kudia
 */

#include "ezsp.h"

namespace zb_ezsp {

/**
 * Ping device
 */
void Ezsp::echo() {
    zb_ezsp::echo ech;
    ech.dataLength = 10;
    for(int i=0; i<ech.dataLength; i++){
        ech.data[i] = '0' + 1;
    }

    add2output<zb_ezsp::echo>(zb_ezsp::EId::ID_Echo, ech);
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
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_stopScan, no_prm);
}

/**
 * Allows the NCP to respond with a pending callback
 */
void Ezsp::allowCallback(){
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_callback, no_prm);
}

/**
 * Value get/set
 */
void Ezsp::getValue(const EzspValueId id){
    zb_ezsp::value_get_req v_id;
    v_id.valueId = id;
    add2output<zb_ezsp::value_get_req>(zb_ezsp::EId::ID_getValue, v_id);
}


/**
 * Configuration value get/set
 */
void Ezsp::getCinfigurationValue(const EzspConfigId id){
    zb_ezsp::configid_get_req v_cfg;
    v_cfg.configId = id;
    add2output<zb_ezsp::configid_get_req>(zb_ezsp::EId::ID_getConfigurationValue, v_cfg);
}

/**
 * Init network
 */
void Ezsp::networkInitExt(const EmberNetworkInitBitmask bitmask/* = EmberNetworkInitBitmask::EMBER_NETWORK_INIT_NO_OPTIONS*/) {
    zb_ezsp::EmberNetworkInitStruct net;
    net.bitmask = bitmask;
    add2output<zb_ezsp::EmberNetworkInitStruct>(zb_ezsp::EId::ID_networkInitExtended, net);
}

void Ezsp::networkInit(){
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_networkInit, no_prm);
}

void Ezsp::formNetwork(){
    EmberNetworkParameters netPrm;
    //uint8_t extPAN[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t extPAN[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //External PAN ID
    memcpy(netPrm.extendedPanId, extPAN, sizeof(extPAN));
    //PAN
    netPrm.panId = 0xA0B0;
    netPrm.radioTxPower = 8;  //dBm
    netPrm.radioChannel = 11;
    netPrm.joinMethod = EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION;
    netPrm.nwkManagerId = 0;
    netPrm.nwkUpdateId = 0;
    netPrm.channels = 0;

    add2output<zb_ezsp::EmberNetworkParameters>(zb_ezsp::EId::ID_formNetwork, netPrm);
}

/**
 * Causes the stack to leave the current network. This generates a stackStatusHandler callback to indicate that the network
 * is down. The radio will not be used until after sending a formNetwork or joinNetwork command.
 */
void Ezsp::leaveNetwork(){
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_leaveNetwork, no_prm);
}

/**
 * A value of 0x00 disables joining. A value of 0xFF enables joining. Any other value enables joining for that number of seconds.
 */
void Ezsp::permitJoining(const uint8_t duration/*=0xFF*/){
    zb_ezsp::permitJoining permit;
    permit.duration = duration;
    add2output<zb_ezsp::permitJoining>(zb_ezsp::EId::ID_permitJoining, permit);
}

void Ezsp::getNetworkParameters(){
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getNetworkParameters, no_prm);
}

/**
 * Security
 */
void Ezsp::setInitialSecurityState(EmberCurrentSecurityBitmask bitmask/* = EmberCurrentSecurityBitmask::EMBER_STANDARD_SECURITY_MODE*/){
    EmberCurrentSecurityState secSt;
    secSt.bitmask = bitmask;
    memset(secSt.trustCenterLongAddress, 0x00, sizeof(secSt.trustCenterLongAddress));
    add2output<zb_ezsp::EmberCurrentSecurityState>(zb_ezsp::EId::ID_setInitialSecurityState, secSt);
}

void Ezsp::getCurrentSecurityState(){
    zb_ezsp::no_params no_prm;
    add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getCurrentSecurityState, no_prm);
}

}
