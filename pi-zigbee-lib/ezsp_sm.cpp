/**
 * ezsp_sm.cpp
 *
 *  EZSP State machine.
 *
 *  Created on: May 27, 2020
 *      Author: Denis Kudia
 */

#include "ezsp_sm.h"
#include "ezsp.h"

namespace zb_ezsp {


void Ezsp::worker(Ezsp* p_ezsp){

    bool finishing = false;
    p_ezsp->activate();     //Run UART working thread

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + "Worker started.");

    auto fn = [p_ezsp]{return (p_ezsp->is_stop_signal() | !p_ezsp->_events->is_empty());};
    auto fn_msg = [p_ezsp]{return (!p_ezsp->_events->is_empty());};

    while(true){

        {
            std::unique_lock<std::mutex> lk(p_ezsp->cv_m);
            if(finishing)
                p_ezsp->cv.wait(lk, fn_msg);
            else
                p_ezsp->cv.wait(lk, fn);
        }

        /**
         * If stop signal detected and we is not processing this signal again
         */
        if(p_ezsp->is_stop_signal() && !finishing ){
            if(p_ezsp->state() == Ezsp_State::SM_UP_and_Ready){ //If network UP - leave network first
                logger::log(logger::LLOG::INFO, "ezsp", std::string(__func__) + "Worker. Stop signal detected on Net UP state.");
                finishing = true;
                p_ezsp->leaveNetwork();
                continue;
            }
            else{ //else just finish
                logger::log(logger::LLOG::INFO, "ezsp", std::string(__func__) + "Worker. Stop signal detected");
                break;
            }
        }

        /**
         * Process evens received from low level or as command from outside
         */
        std::shared_ptr<EzspEvent> evt = p_ezsp->_events->get();
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + evt->to_string());

        switch(evt->id()){
            case Ezsp_SM_Event::EVT_Start:
            {
                if(p_ezsp->state() == Ezsp_State::SM_Initial){
                    p_ezsp->uart_start();
                }
            }
            break;
            case Ezsp_SM_Event::EVT_UART_UP:
            {
                if(p_ezsp->state() == Ezsp_State::SM_Initial){

                    /**
                     * Get/Set some configuration values
                     */
                    //p_ezsp->setCinfigurationValue(zb_ezsp::EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT, 24);
                    //p_ezsp->setCinfigurationValue(EzspConfigId::EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT, 6000);
                    //p_ezsp->getCinfigurationValue(EzspConfigId::EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT);
                    p_ezsp->setCinfigurationValue(EzspConfigId::EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE, 4);
                    p_ezsp->setCinfigurationValue(EzspConfigId::EZSP_CONFIG_KEY_TABLE_SIZE, 6);

                    //Low level is UP
                    p_ezsp->set_state(Ezsp_State::SM_Init_Network);

                    /**
                     * Create network
                     */
                    if(p_ezsp->is_coordinator()){
                        p_ezsp->setInitialSecurityState();
                    }
                    else{
                        //find network and join
                    }
                }
            }
            break;
            /**
             * Something wrong
             */
            case Ezsp_SM_Event::EVT_NEIGHBOR_CONN:
            {

            }
            break;
            /**
             *
             */
            case Ezsp_SM_Event::EVT_NET_STATUS:
            {
                if(evt->get_status() == EmberStatus::EMBER_NETWORK_UP){
                    p_ezsp->set_state(Ezsp_State::SM_UP_and_Ready);

                    /**
                     * If I would like to be trust center itself but not become it yet then ask key and become Trust Center
                     */
                    if(p_ezsp->is_trust_center() && !p_ezsp->is_become_trust_center()){
                        p_ezsp->BecomeTrustCenter();
                    }
                    else{
                        /**
                         * Allow join to network
                         */
                        if(p_ezsp->is_coordinator()){
                            p_ezsp->permitJoining(); //permanently
                        }
                    }
                }
                else if(evt->get_status() == EmberStatus::EMBER_NETWORK_DOWN){
                    p_ezsp->set_state(Ezsp_State::SM_Init_Network);
                }
            }
            break;
            case Ezsp_SM_Event::EVT_TRUST_CENTER:
            {
                if(p_ezsp->state() == Ezsp_State::SM_UP_and_Ready){
                    /**
                     * Allow join to network
                     */
                    if(p_ezsp->is_coordinator()){
                        p_ezsp->permitJoining(); //permanently
                    }
                }
            }
            break;
        }

        /**
         *
         */
        if(finishing && p_ezsp->state() == Ezsp_State::SM_Init_Network){
            break;
        }

    }

    if(p_ezsp->state() != Ezsp_State::SM_Initial){
        p_ezsp->uart_stop();
    }
}


}