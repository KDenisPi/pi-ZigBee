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

#include "ezsp_frame.h"
#include "ezsp_sm.h"

#include "uart.h"
#include "uart_efr_buff.h"

namespace zb_ezsp {

/**
 * EZSP implementation
 */
class Ezsp : public piutils::Threaded {
public:
    Ezsp(const bool debug_mode=true) : _seq(0), _debug(debug_mode), frame_received(nullptr) {

        _uart = std::make_shared<zb_uart::ZBUart>(debug_mode);
        _events = std::make_shared<EventBuff>(20);

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


    /**
     * Configuration value get/set
     */
    void getCinfigurationValue(const EzspConfigId id);

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

    /**
     * Security
     */
    void setInitialSecurityState(EmberCurrentSecurityBitmask bitmask = EmberCurrentSecurityBitmask::EMBER_STANDARD_SECURITY_MODE);
    void getCurrentSecurityState();


    /**
     * Messaging
     */
    void sendUnicast();

    /**
     *
     */
    const std::string neighbors() {
        return list_childs();
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

        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ******* Create frame ID:" + std::to_string((uint16_t)id) + " SEQ: " +  std::to_string((uint16_t)efr->seq()));


        if(is_debug()){
            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + efr->to_string());
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
        //TODO: Temporal solution
        _networks[0] = std::make_shared<EmberNetworkParameters>();

        uint8_t ex_pan[8] = {0xC4,0x9D,0xF8,0x5C,0x57,0x01,0x6C,0xA7};
        _networks[0]->set_ext_pan(ex_pan);
        _networks[0]->panId = 41136;
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

    /**
     * Child operations (Add, Delete, Print, Get)
     */
    void add_child(const std::shared_ptr<childJoinHandler> child) {
        if(_childs.find(child->childId) == _childs.end()){
            _childs[child->childId] = child;
        }
    }

    void del_child(const EmberNodeId child_id){
        auto child = _childs.find(child_id);
        if(child != _childs.end()){
            _childs.erase(child);
        }
    }

    const std::string list_childs() {
        std::string result;
        for (auto it = _childs.begin(); it != _childs.end(); ++it) {
            result += it->second->to_string() + "\n";
        }
        return result;
    }

    const EmberNodeId get_child() {
        if(_childs.empty())
            return 0;
        auto it = _childs.begin();
        return it->first;
    }

    const size_t count_child() {
        return _childs.size();
    }


private:
    uint8_t _seq;
    bool _debug;
    std::shared_ptr<zb_uart::ZBUart> _uart = std::make_shared<zb_uart::ZBUart>(true);

    Ezsp_State _state = Ezsp_State::SM_Initial;
    std::shared_ptr<EventBuff> _events;

    EmberNodeType _node_type = EmberNodeType::EMBER_COORDINATOR;
    EmberEUI64 _eui64;   // The 64-bit ID
    EmberNodeId _nodeId;  // The 16-bit ID

    std::array<std::shared_ptr<EmberNetworkParameters>, 4> _networks;
    std::map<EmberNodeId, std::shared_ptr<childJoinHandler>> _childs;

    /**
     * Detect Frame ID
     */
    const id_type detect_id(const uint8_t* buff, size_t len) {
        id_type id = 0xFF; //fake code
        size_t pos = 2;

        if(EzspVersion::ver() < 8){
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
};

}

#endif
