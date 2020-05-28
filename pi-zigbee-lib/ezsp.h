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

#include "ezsp_frame.h"
#include "uart.h"
#include "uart_efr_buff.h"

namespace zb_ezsp {

/**
 * EZSP implementation
 */
class Ezsp {
public:
    Ezsp(const bool debug_mode=true) : _seq(0), _debug(debug_mode), frame_received(nullptr) {

        _uart = std::make_shared<zb_uart::ZBUart>(debug_mode);

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

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Create frame ID:" + std::to_string((uint16_t)id) + " SEQ: " +  std::to_string((uint16_t)efr->seq()));


        if(is_debug()){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + efr->to_string());
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
        uint8_t w_buff[140];
        zb_ezsp::ver_req ver;

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Connected: " + std::to_string(conn));

        if(!conn){ //Low level error
            return;
        }

        //create EZSP Version frame
        ver._ver = zb_ezsp::EzspVersion::ver();
        add2output<zb_ezsp::ver_req>(zb_ezsp::EId::ID_version, ver);
    }

    /**
     *
     */
    void callback_eframe_received(const zb_uart::EFramePtr& efr_raw){
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Received frame Len: " + std::to_string(efr_raw->len()));

        std::shared_ptr<zb_ezsp::EFrame> ef = std::make_shared<zb_ezsp::EFrame>();

        auto id = detect_id(efr_raw->data(), efr_raw->len());
        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Detected ID " + std::to_string((uint16_t)id));

        switch(id){
            case EId::ID_version:
            {
                auto p_ver = ef->load<zb_ezsp::ezsp_ver_resp>(efr_raw->data(), efr_raw->len());
                notify(EId::ID_version, p_ver.to_string());

                /**
                 * Get node information
                 */
                get_nodeId_Eui64();
                get_nodeId();
            }
            break;
            /**
             * Group of EmberStatus only responses
             */
            case EId::ID_startScan:
            case EId::ID_stopScan:
            case EId::ID_invalidCommand:
            case EId::ID_networkInit:
            case EId::ID_networkInitExtended:
            case EId::ID_formNetwork:
            case EId::ID_stackStatusHandler:
            case EId::ID_permitJoining:
            case EId::ID_setInitialSecurityState:
            case EId::ID_leaveNetwork:
            {
                auto p_status = ef->load<zb_ezsp::ember_status>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_status.to_string());
            }
            break;
            /**
             *Group no data responses
             */
            case EId::ID_noCallbacks:
            {
                notify((EId)id, std::string());
            }
            break;
            /**
             * Reports that a network was found as a result of a prior call to startScan. Gives the network parameters useful for deciding which network to join.
             */
            case EId::ID_networkFoundHandler:
            {
                if(is_debug()){
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " networkFoundHandler: " + zb_ezsp::Conv::print_buff(efr_raw->data(), efr_raw->len()));
                }

                auto p_network = ef->load<zb_ezsp::networkFoundHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_network.to_string());
            }
            break;
            case EId::ID_childJoinHandler:
            {
                auto p_child = ef->load<zb_ezsp::childJoinHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_child.to_string());
            }
            break;
            case EId::ID_getEui64:
            {
                auto p_eui64 = ef->load<zb_ezsp::Eui64>(efr_raw->data(), efr_raw->len());
                memcpy(this->_eui64, p_eui64.eui64, sizeof(EmberEUI64));
                notify((EId)id, Conv::Eui64_to_string(p_eui64.eui64));

            }
            break;
            case EId::ID_getNodeId:
            {
                auto p_nodeid = ef->load<zb_ezsp::NodeId>(efr_raw->data(), efr_raw->len());
                this->_nodeId = p_nodeid.nodeId;
                notify((EId)id, Conv::to_string(this->_nodeId));
            }
            break;

            case EId::ID_networkState:
            {
                auto p_netstate = ef->load<zb_ezsp::networkState>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_netstate.to_string());
            }
            break;
            case EId::ID_getNetworkParameters:
            {
                auto p_netstate = ef->load<zb_ezsp::getNetworkParameters_resp>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_netstate.to_string());
            }
            break;
            case EId::ID_scanCompleteHandler:
            {
                auto p_handler = ef->load<zb_ezsp::scanCompleteHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_handler.to_string());
            }
            break;
            case EId::ID_energyScanResultHandler:
            {
                auto p_channel = ef->load<zb_ezsp::energyScanResultHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_channel.to_string());
            }
            break;
            case EId::ID_getConfigurationValue:
            {
                auto p_conf = ef->load<zb_ezsp::configid_get_resp>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_conf.to_string());
            }
            break;
            case EId::ID_getCurrentSecurityState:
            {
                auto p_conf = ef->load<zb_ezsp::getCurrentSecurityState>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_conf.to_string());
            }
            break;
            case EId::ID_Echo:
            {
                auto p_echo = ef->load<zb_ezsp::echo>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_echo.to_string());
            }
            break;
            case EId::ID_getValue:
            {
                auto p_status = ef->load<zb_ezsp::value_get_resp>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_status.to_string());
            }
            break;
            /**
             * EZSP status only
             */
            case EId::ID_setValue:
            case EId::ID_setConfigurationValue:
            {
                auto p_status = ef->load<zb_ezsp::ezsp_status>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_status.to_string());
            }
            break;

            default:
                notify((EId)id, std::string("Not supported yet"));
        }
    }

    const bool is_debug() const {
        return _debug;
    }

public:
    /**
     *
     */
    void start() {
        _uart->start();
    }

    void stop() {
        _uart->stop();
    }

    /**
     *
     */
    void activate() {
        _uart->set_activate(true);
    }

    EmberEUI64 _eui64;   // The 64-bit ID
    EmberNodeId _nodeId;  // The 16-bit ID

    const uint16_t node_id() const {
        return _nodeId;
    }

    const uint8_t* node_id64() const {
        return _eui64;
    }

private:
    uint8_t _seq;
    bool _debug;
    std::shared_ptr<zb_uart::ZBUart> _uart = std::make_shared<zb_uart::ZBUart>(true);


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
