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

#include "ezsp_frame.h"
#include "uart.h"
#include "uart_efr_buff.h"

namespace zb_ezsp {

/**
 * EZSP implementation
 */
class Ezsp {
public:
    Ezsp(const bool debug_mode=true) : _seq(0), _debug(debug_mode) {

        _uart = std::make_shared<zb_uart::ZBUart>(debug_mode);

        _uart->callback_connected = std::bind(&Ezsp::callback_connected, this, std::placeholders::_1);
        _uart->callback_eframe_received = std::bind(&Ezsp::callback_eframe_received, this, std::placeholders::_1);
    }

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

protected:
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
        std::shared_ptr<zb_ezsp::EFrame> ef_ver = std::make_shared<zb_ezsp::EFrame>(zb_ezsp::EId::ID_version);
        ef_ver->set_seq(seq_next());

        if(is_debug()){
            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + ef_ver->to_string());
        }

        //Convert EZSP frame to buffer
        memset(w_buff, 0x00, sizeof(w_buff));
        size_t wr_len = ef_ver->put<zb_ezsp::ver_req>(w_buff, 0, ver);

        //Prepare object for UART sending as DATA frame
        zb_uart::EFramePtr e_fr = std::make_shared<zb_uart::EFData>(wr_len, w_buff);

        //Add to queue
        _uart->put_ezsp_frame(e_fr);
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
                }
                break;
            case EId::ID_Echo:
                {
                    auto p_echo = ef->load<zb_ezsp::echo>(efr_raw->data(), efr_raw->len());
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
