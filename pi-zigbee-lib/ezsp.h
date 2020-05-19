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
    void echo() {
        zb_ezsp::echo ech;
        ech.dataLength = 10;
        for(int i=0; i<ech.dataLength; i++){
            ech.data[i] = '0' + 1;
        }

        add2output<zb_ezsp::echo>(zb_ezsp::EId::ID_Echo, ech);
    }

    /**
     * This function will start a scan
     * ch2scan - number channels to scan (15 channels). OxFF - scan all
     * channels - array of channels have to be scan. Not used if ch2scan = 0xFF
     * duration - Sets the exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods.
     */
    void startScan(const zb_ezsp::EzspNetworkScanType scanType, const uint8_t ch2scan, const uint8_t* channels = nullptr, const uint8_t duration = 4) {
        zb_ezsp::start_scan scan;

        if(ch2scan == 0xFF){
            scan.channelMask = 0x07FFF800;
        }
        else{
            for(int i =0; i<ch2scan; i++){
                scan.channelMask |= (1<<channels[i]);
                scan.channelMask &= 0x07FFF800; //clear incorrect channels
            }
        }

        scan.scanType = scanType;
        scan.duration = duration;
        add2output<zb_ezsp::start_scan>(zb_ezsp::EId::ID_startScan, scan);
    }

    /**
     * Allows the NCP to respond with a pending callback
     */
    void allowCallback(){
        zb_ezsp::no_params no_prm;
        add2output<zb_ezsp::no_params>(zb_ezsp::EId::ID_callback, no_prm);
    }

protected:
    /**
     * Add frame to output queue
     */
    template<typename T>
    void add2output(zb_ezsp::EId id, T& obj){
        uint8_t w_buff[140];

        std::shared_ptr<zb_ezsp::EFrame> efr = std::make_shared<zb_ezsp::EFrame>(id);
        efr->set_seq(seq_next());

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
            }
            break;
            /**
             * Group of EmberStatus only responses
             */
            case EId::ID_startScan:
            case EId::ID_invalidCommand:
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
            //Reports that a network was found as a result of a prior call to startScan. Gives the network parameters useful for deciding which network to join.
            case EId::ID_networkFoundHandler:
            {
                auto p_network = ef->load<zb_ezsp::networkFoundHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_network.to_string());
            }
            break;
            case EId::ID_energyScanResultHandler:
            {
                auto p_channel = ef->load<zb_ezsp::energyScanResultHandler>(efr_raw->data(), efr_raw->len());
                notify((EId)id, p_channel.to_string());
            }
            break;
            case EId::ID_Echo:
            {
                auto p_echo = ef->load<zb_ezsp::echo>(efr_raw->data(), efr_raw->len());
                notify(EId::ID_Echo, p_echo.to_string());
            }
            break;
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
