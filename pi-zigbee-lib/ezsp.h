/**
 * ezsp.h
 *
 *  EZSP Implementation
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_H_
#define PI_ZIGBEE_LIB_EZSP_H_

#include <functional>
#include <memory>
#include <array>

#include "Threaded.h"

#include "ezsp_childs.h"
#include "zcl.h"
#include "zdp.h"
#include "ezsp_frame.h"
#include "ezsp_sm.h"
#include "ezsp_db_json.h"
#include "aps_defs.h"

#include "uart.h"
#include "uart_efr_buff.h"

namespace zb_ezsp {

/**
 * EZSP implementation
 */

class Ezsp : public piutils::Threaded {
public:
    Ezsp(const bool debug_mode=true, const std::string config = "./config.json") : _seq(0), _debug(debug_mode), frame_received(nullptr), _config_prm(config) {

        _uart = std::make_shared<zb_uart::ZBUart>(debug_mode);
        _events = std::make_shared<EventBuff>(20);
        _childs = std::make_shared<childs::Childs>();

        load_config();

        _uart->callback_connected = std::bind(&Ezsp::callback_connected, this, std::placeholders::_1);
        _uart->callback_eframe_received = std::bind(&Ezsp::callback_eframe_received, this, std::placeholders::_1);
    }

    /**
     *
     */
    ~Ezsp() {
        stop();
    }

    Ezsp(Ezsp&) = delete;
    Ezsp(Ezsp&&) = delete;
    Ezsp& operator=(const Ezsp&) = delete;
    Ezsp& operator=(Ezsp&&) = delete;

    static const uint8_t EMBER_CHILD_TABLE_SIZE = 64;

    /**
     *
     */
    void add_event(const EventPtr evt){
        std::lock_guard<std::mutex> lk(cv_m);
        _events->put(evt);
        cv.notify_one();
    }

    /**
     * Sequence number
     */
    const uint8_t seq_next() {
        if(_seq == 0xFF) _seq = 0;
        return _seq++;
    }

    const uint8_t seq() const {
        return _seq;
    }

    /**
     * Callback for recived frames
     * TODO: Implement subsribers
     */
    std::function<void(const EId id, const std::string)> frame_received;

    void notify(const EId id, const std::string info) const {
        char buff[10];
        std::sprintf(buff, "0x%02X", id);
        logger::log(logger::LLOG::NECECCARY, "ezsp", std::string(__func__) + " ID: " + std::string(buff) + " " + info);

        if(frame_received != nullptr){
            frame_received(id, info);
        }
    }


    /**
     * High level interface functions
     */

    /**
     * Ping device
     */
    void echo();

    /**
     * Get node ID
     */
    void get_nodeId_Eui64();
    void get_nodeId();

    /**
     * This function will start a scan
     * ch2scan - number channels to scan (15 channels, from 11 to 26). OxFF - scan all
     * channels - array of channels have to be scan. Not used if ch2scan = 0xFF
     * duration - Sets the exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods.
     */
    void startScan(const zb_ezsp::EzspNetworkScanType scanType, const uint8_t ch2scan, const uint8_t* channels = nullptr, const uint8_t duration = 4);
    void stopScan();

    /**
     * Allows the NCP to respond with a pending callback
     */
    void allowCallback();

    /**
     * Value get/set
     */
    void getValue(const EzspValueId id);
    void getValueTokenStackNodeData();

    /**
     * Configuration value get/set
     */
    void getCinfigurationValue(const uint8_t id);
    void setCinfigurationValue(const uint8_t id, const uint16_t value);
    /**
     * Init network
     */
    void networkInitExt(const EmberNetworkInitBitmask bitmask = EmberNetworkInitBitmask::EMBER_NETWORK_INIT_NO_OPTIONS);
    void networkInit();
    void formNetwork();

    /**
     * Causes the stack to leave the current network. This generates a stackStatusHandler callback to indicate that the network
     * is down. The radio will not be used until after sending a formNetwork or joinNetwork command.
     */
    void leaveNetwork();

    /**
     * A value of 0x00 disables joining. A value of 0xFF enables joining. Any other value enables joining for that number of seconds.
     */
    void permitJoining(const uint8_t duration=0xFF);
    void getNetworkParameters();

    void getParentChildParameters(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_getParentChildParameters, no_prm);
    }

    void neighborCount(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_neighborCount, no_prm);
    }

    /**
     * Security
     */
    void setInitialSecurityState();
    void getCurrentSecurityState();

    void getKey(const EmberKeyType key = EmberKeyTypeEnum::EMBER_CURRENT_NETWORK_KEY){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

        zb_ezsp::uint8t_value int8_value;
        int8_value.value = key;
        add2output<zb_ezsp::uint8t_value>(zb_ezsp::EId::ID_getKey, int8_value);
    }

    void BecomeTrustCenter(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + _key_network.to_string());
        zb_ezsp::BecomeTrustCenter trCenter;

        memcpy(trCenter.key, _key_network.key, sizeof(trCenter.key));
        add2output<zb_ezsp::BecomeTrustCenter>(zb_ezsp::EId::ID_becomeTrustCenter, trCenter);
    }

    void send_unicastNwkKeyUpdate(const EmberNodeId& nodeId, const EmberEUI64& node_eui64){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " nodeId: " + Conv::to_string(nodeId) + " EUI64: " + Conv::to_string(node_eui64));

        zb_ezsp::unicastNwkKeyUpdate keyUpdate;

        keyUpdate.destShort = nodeId;
        Conv::copy(keyUpdate.destLong, node_eui64);
        _key_network.copy_key(keyUpdate.key);

        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + keyUpdate.to_string());

        add2output<zb_ezsp::unicastNwkKeyUpdate>(zb_ezsp::EId::ID_unicastNwkKeyUpdate, keyUpdate);
    }

    void broadcastNextNetworkKey(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

        keyData key;   //An optional pointer to a 16-byte encryption key (EMBER_ENCRYPTION_KEY_SIZE). An all zero key may be passed in, which
                            // will cause the stack to randomly generate a new key.
        memset(key.key, 0x00, sizeof(EmberKeyData));
        add2output<zb_ezsp::keyData>(zb_ezsp::EId::ID_broadcastNextNetworkKey, key);
    }

    void broadcastNetworkKeySwitch(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));
        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_broadcastNetworkKeySwitch, no_prm);
    }

    void clearKeyTable(){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));
        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_clearKeyTable, no_prm);
    }

    /**
     * Messaging
     */
    void sendUnicast(const EmberNodeId node_id,
                    const zb_aps::ApsPayload& data,
                    const uint8_t cluster = 0,
                    const uint8_t profile = 0,
                    const uint8_t srcEp = 0,
                    const uint8_t dstEp = 0,
                    const uint8_t seq = 0,
                    const uint8_t tag=0
                    );
    void sendZcl(const EmberEUI64& childEui64);

    void setExtendedTimeout(const EmberEUI64& remoteNodeId, bool extTimeout = true);
    void getExtendedTimeout(const EmberEUI64& remoteNodeId);

    void lookupEui64ByNodeId(const EmberNodeId nodeId){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));
        zb_ezsp::NodeId node_id;
        node_id.nodeId = nodeId;
        add2output<zb_ezsp::NodeId>(zb_ezsp::EId::ID_lookupEui64ByNodeId, node_id);
    }

    /**
     * Get Route Table Entry
     *
     * @params index The index of the route table entry of interest.
     */
    void getRouteTableEntry(const uint8_t& index);

    /**
     * APS Messages payload
     */
    //void sendApsTransportKey(const childs::child_info& child);

    /**
     *
     */
    const std::string neighbors() {
        return _childs->list_childs();
    }

    void get_childData(const uint8_t index = 0){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Index: " + std::to_string(index));

        if(index >= EMBER_CHILD_TABLE_SIZE){ //Low level error
            logger::log(logger::LLOG::ERROR, "ezsp", std::string(__func__) + " Invalid Index: " + std::to_string(index));
            return;
        }

        //create EZSP Version frame
        zb_ezsp::get_by_index cldIdx;
        cldIdx.index = index;
        add2output<zb_ezsp::get_by_index>(zb_ezsp::EId::ID_getChildData, cldIdx);
    }

    /**
     * Binding
     */
    void clearBindingTable() {
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__));

        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_clearBindingTable, no_prm);
    }

    void setBinding(const EmberEUI64& childEui64);

    void getBinding(const uint8_t index){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Index: " + std::to_string(index));

        zb_ezsp::get_by_index get_bind;
        get_bind.index = index;
        add2output<zb_ezsp::get_by_index>(zb_ezsp::EId::ID_getBinding, get_bind);
    }

    void getMfgToken(const EzspMfgTokenId token){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Token: " + std::to_string(token));
        zb_ezsp::get_by_index get_Mfg_token;
        get_Mfg_token.index = token;
        add2output<zb_ezsp::get_by_index>(zb_ezsp::EId::ID_getMfgToken, get_Mfg_token);
    }

    /**
     * Request manufacturer configuration
     */
    void getDeviceInfo(){
        getMfgToken(EzspMfgTokenId::EZSP_MFG_CUSTOM_VERSION);
        getMfgToken(EzspMfgTokenId::EZSP_MFG_STRING);
        getMfgToken(EzspMfgTokenId::EZSP_MFG_BOARD_NAME);
        getMfgToken(EzspMfgTokenId::EZSP_MFG_MANUF_ID);
        getMfgToken(EzspMfgTokenId::EZSP_MFG_PHY_CONFIG);
        getMfgToken(EzspMfgTokenId::EZSP_MFG_BOOTLOAD_AES_KEY);
    }

    /**
     * EZSP SM worker
     */
    static void worker(Ezsp* p_ezsp);

protected:
    /**
     * Add frame to output queue
     */
    template<typename T>
    void add2output(zb_ezsp::EId id, T& obj){
        uint8_t w_buff[140];

        std::shared_ptr<zb_ezsp::EFrame> efr = std::make_shared<zb_ezsp::EFrame>(id);
        efr->set_seq(seq_next());

        char buff[10];
        std::sprintf(buff, "0x%04X", (uint16_t)id);
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Create frame ID:" + std::string(buff) + " " + get_id_name(id) + " SEQ: " +  std::to_string((uint16_t)efr->seq()));


        if(is_debug()){
            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + get_id_name(id) + " " + efr->to_string());
        }

        //Convert EZSP frame to buffer
        memset(w_buff, 0x00, sizeof(w_buff));
        size_t wr_len = efr->put<T>(w_buff, 0, obj);

        //Prepare object for UART sending as DATA frame
        zb_uart::EFramePtr e_fr = std::make_shared<zb_uart::EFData>(wr_len, w_buff);

        //Add to queue
        _uart->put_ezsp_frame(e_fr);
    }

    /**
     *
     */
    void callback_connected(bool conn){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Connected: " + std::to_string(conn));

        if(!conn){ //Low level error
            return;
        }

        //create EZSP Version frame
        zb_ezsp::ver_req ver;
        ver._ver = zb_ezsp::EzspVersion::ver();
        add2output<zb_ezsp::ver_req>(zb_ezsp::EId::ID_version, ver);
    }

    /**
     * Process callbacks from UART
     */
    void callback_eframe_received(const zb_uart::EFramePtr& efr_raw);

    const bool is_debug() const {
        return _debug;
    }

public:
    /**
     *
     */
    bool start() {
        //Add Start even to the queue
        std::shared_ptr<EzspEvent> evt = std::make_shared<EzspEvent>(EVT_Start);
        add_event(evt);

        //Start working thread
        return piutils::Threaded::start<Ezsp>(this);
    }

    /**
     *
     */
    const uint16_t node_id() const {
        return _nodeId;
    }

    const uint8_t* node_id64() const {
        return _eui64;
    }

    const Ezsp_State state() const {
        return _state;
    }

protected:
    void uart_start() {
        _uart->start();
    }

    void uart_stop(){
        _uart->stop();
    }

    void activate() {
        _uart->set_activate(true);
    }

    const EventPtr& get_event(){
        return _events->get();
    }

    void set_state(const Ezsp_State st){
        logger::log(logger::LLOG::INFO, "ezsp", std::string(__func__) + " From: " + std::to_string(_state) + " To: " + std::to_string(st));
        _state = st;
    }

    /**
     * Load configuration and another data detected during previos session
     */
    void load_config(){

        /**
         * Network information
         */
        _config.load(_config_prm);
        _config.load_networks(_networks);
    }

    /**
     * Get node type
     */
    const EmberNodeType node_type() const {
        return _node_type;
    }

    bool is_coordinator() const {
        return (node_type() == EmberNodeType::EMBER_COORDINATOR);
    }

    bool is_trust_center() const {
        return _trust_center;
    }

    bool is_become_trust_center() const {
        return _become_trust_center;
    }

    void become_trust_center(const bool become_trust = true){
        _become_trust_center = become_trust;
    }


    const EmberKeyData DefaultGlobalTrustCenterLinkKey = {'Z','i','g','B','e','e','A','l','l','i','a','n','c','e','0','9'};

private:
    uint8_t _seq;
    bool _debug;
    std::shared_ptr<zb_uart::ZBUart> _uart = std::make_shared<zb_uart::ZBUart>(true);

    Ezsp_State _state = Ezsp_State::SM_Initial;
    std::shared_ptr<EventBuff> _events;

    /**
     * Network parameters
     */
    EmberNodeType _node_type = EmberNodeType::EMBER_COORDINATOR;
    EmberEUI64 _eui64;                      // The 64-bit ID
    EmberNodeId _nodeId;                    // The 16-bit ID
    bool _trust_center = false; //true;      //Is I would like to be trust center itself
    bool _become_trust_center = false;      //If I am trust center already

    /**
     * Security. Keys.
     */
    EmberKeyStruct _key_network;              // The current active Network Key used by all devices in the network.
    EmberKeyStruct _key_trust_center_link;    // A shared key between the Trust Center and a device.

    net_array _networks;
    std::shared_ptr<childs::Childs> _childs;

    std::string _config_prm; //config parameters (file name for JSON)
    EzspDbJson _config;

    /**
     * Detect Frame ID
     */
    const id_type detect_id(const uint8_t* buff, size_t len) {
        id_type id = 0xFF; //fake code
        size_t pos = 2;

        if(EzspVersion::ver() > 5){
            id = Conv::get_byte(buff, pos);
            if(id == 0xFF && (EzspVersion::ver() >= 5 && EzspVersion::ver() < 8)){
                pos++; //skip one byte
                id = Conv::get_byte(buff, pos);
            }
        }
        else { //Version 8 and higher
            pos = 3;
            id = Conv::get_short(buff, pos);
        }

        return id;
    }

    static std::map<id_type, std::string> _frameId2String;

public:
    static const std::string get_id_name(const id_type id){
        auto id_info = _frameId2String.find(id);
        if(id_info != _frameId2String.end()){
            return id_info->second;
        }

        return std::string("Unknown");
    }

};

}

#endif
