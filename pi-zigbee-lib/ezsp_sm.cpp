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

    p_ezsp->activate();     //Run UART working thread

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + "Worker started.");

    auto fn = [p_ezsp]{return (p_ezsp->is_stop_signal() | !p_ezsp->_events->is_empty());};

    while(!p_ezsp->is_stop_signal()){

        {
            std::unique_lock<std::mutex> lk(p_ezsp->cv_m);
            p_ezsp->cv.wait(lk, fn);
        }

        if(p_ezsp->is_stop_signal()){
            logger::log(logger::LLOG::INFO, "ezsp", std::string(__func__) + "Worker. Stop signal detected");
            break;
        }

        /**
         * Process evens received from low level or as command from outside
         */
        std::shared_ptr<EzspEvent> evt = p_ezsp->_events->get();

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
                    //Low level is UP
                    p_ezsp->set_state(Ezsp_State::SM_Init_Network);

                    /**
                     * Create network
                     */
                    if(p_ezsp->node_type() == EmberNodeType::EMBER_COORDINATOR){
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
                }
                else if(evt->get_status() == EmberStatus::EMBER_NETWORK_DOWN){
                    p_ezsp->set_state(Ezsp_State::SM_UP_and_Ready);
                }
            }
            break;
        }

    }

    if(p_ezsp->state() != Ezsp_State::SM_Initial){
        p_ezsp->uart_stop();
    }
}


}