/**
 * UART.cpp
 *
 *  Created on: Apr 1, 2020
 *      Author: Denis Kudia
 */
#include "uart.h"
#include "ezsp.h"

namespace zb_uart {

    /**
     * UART worker function
     *
     * 1. Wait until UART not connected to device (is_device_connected)
     * 2. Initialize device
     * 2.1 Send RST frame
     * 2.2 Receive correct RSTACK frame
     * 2.3 If RSTACK frame will not receive during T_RSTACK_MAX repeat from 2.1
     * 2.4 Mark UART is on Connected state
     * 2.5 Notify EZSP
     * 3. Process wait for input data and process EZSP messages (send, receive answer)
     *
     */
    void ZBUart::worker(ZBUart* p_uart){

        uint8_t w_buff[140], r_buff[140];
        bool result = false;
        int wr_res = 0, r_res =0;
        std::shared_ptr<ZBUart_Info> info = p_uart->get_session_info();

        logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + "Worker started.");

        auto fn = [p_uart]{return (p_uart->is_stop_signal() | p_uart->is_activated());};

        while(!p_uart->is_stop_signal()){

            {
                logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + "Wait signal.");
                logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + std::string(" Signal Activated: ") + std::to_string(p_uart->is_activated()) +
                    std::string(" Stop: ") + std::to_string(p_uart->is_stop_signal()));

                std::unique_lock<std::mutex> lk(p_uart->cv_m);
                p_uart->cv.wait(lk, fn);

                logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + std::string(" Signal detected Activated: ") + std::to_string(p_uart->is_activated()) +
                    std::string(" Stop: ") + std::to_string(p_uart->is_stop_signal()));
            }

            if(p_uart->is_stop_signal()){
                logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + "Worker. Stop signal detected");
                break;
            }

            /**
             * Initialize devive before working
             */
            if(!info->is_connected()){
                result = false;

                if(info->is_device_connected() || p_uart->connect("/dev/ttyUSB1", 57600)){
                    /**
                     * Initialize device
                     */
                    if(p_uart->init_device(3)){
                        result = true;
                    }//initilize device

                    //Notify EZSP low level connected
                    if(p_uart->callback_connected != nullptr)
                        p_uart->callback_connected(result);

                }//connect to device
                else{
                    logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not connect to device");
                }
            }//connection state

            //Initialization failed. Posponse processing
            if(!result){
                info->set_activate(false); //stop processing
                continue;
            }

            while(!p_uart->is_stop_signal()){

                /**
                 * Check output queue
                 */
                const EFramePtr e_fr = p_uart->get_ezsp_frame();
                if(e_fr){
                    std::shared_ptr<zb_uart::UFrame> fr = p_uart->compose_data(e_fr->data(), e_fr->len());
                    wr_res = p_uart->send_frame(fr);
                    if(wr_res == 0){
                        logger::log(logger::LLOG::ERROR, "uart", std::string(__func__) + " Could not write to device");
                    }
                }

                /**
                 * Wait data from NCP side
                 */
                memset(r_buff, 0x00, sizeof(r_buff));
                size_t rd_len = sizeof(r_buff);
                r_res = p_uart->read_data(r_buff, rd_len);

                if(r_res == 0 && rd_len == 0){ //timeout
                    logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " Timeout ");
                }
                else if(r_res == 0 && rd_len > 0){
                    /**
                     * Create UART frame and load response data
                     */
                    std::shared_ptr<zb_uart::UFrame> fr_rsv = p_uart->parse(r_buff, rd_len, true);
                    if(fr_rsv){
                        if(p_uart->is_debug()){
                            logger::log(logger::LLOG::DEBUG, "uart", std::string(__func__) + " UART Frame : " + fr_rsv->to_string());
                        }

                        if(fr_rsv->is_DATA()){

                            info->set_ackNum(fr_rsv->frmNum()+1);

                            zb_uart::EFramePtr e_fr = std::make_shared<zb_uart::EFData>(fr_rsv->data_len(), fr_rsv->data());

                            /**
                             * Send ACK
                             */
                            std::shared_ptr<zb_uart::UFrame> fr_ack = p_uart->compose(zb_uart::ftype::ACK);
                            wr_res = p_uart->send_frame(fr_ack);

                            if(p_uart->callback_eframe_received != nullptr)
                                p_uart->callback_eframe_received(e_fr);

                        }
                    }
                }

                sleep(1);
            }

        }//main loop

        /**
         *  Close device connection
         */
        if(info->is_device_connected()){
            p_uart->disconnect();
        }

        logger::log(logger::LLOG::INFO, "uart", std::string(__func__) + "Worker finished.");
    }

}