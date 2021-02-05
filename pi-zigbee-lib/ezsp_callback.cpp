/**
 * ezsp_callback.cpp
 *
 *  EZSP Frame receive callback.
 *
 *  Created on: May 27, 2020
 *      Author: Denis Kudia
 */

#include "ezsp.h"

namespace zb_ezsp {

/**
 *
 */
void Ezsp::callback_eframe_received(const zb_uart::EFramePtr& efr_raw){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Received frame Len: " + std::to_string(efr_raw->len()));

    std::shared_ptr<zb_ezsp::EFrame> ef = std::make_shared<zb_ezsp::EFrame>();

    auto id = detect_id(efr_raw->data(), efr_raw->len());
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Detected ID " + Conv::to_string((uint16_t)id) + " " + get_id_name(id));

    switch(id){
        case EId::ID_version:
        {
            auto p_ver = ef->load<zb_ezsp::ezsp_ver_resp>(efr_raw->data(), efr_raw->len());
            notify(EId::ID_version, p_ver->to_string());

            /**
             * Get node information
             */
            get_nodeId_Eui64();
            get_nodeId();

            if(state() == Ezsp_State::SM_Initial){
                add_event(std::make_shared<EzspEvent>(Ezsp_SM_Event::EVT_UART_UP));
            }
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
        case EId::ID_clearBindingTable:
        case EId::ID_setBinding:
        case EId::ID_becomeTrustCenter:
        case EId::ID_unicastNwkKeyUpdate:
        case EId::ID_clearKeyTable:
        case EId::ID_broadcastNextNetworkKey:
        case EId::ID_broadcastNetworkKeySwitch:
        {
            auto p_status = ef->load<zb_ezsp::ember_status>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_status->to_string());

            if((p_status->status != EmberStatus::EMBER_SUCCESS) && (p_status->status != EMBER_NETWORK_UP) && (p_status->status != EMBER_NETWORK_DOWN)){
                logger::log(logger::LLOG::ERROR, "ezsp", std::string(__func__) + "Error: " + std::to_string((uint16_t)p_status->status) + " Frame ID: " + std::to_string((uint16_t)id));

                std::shared_ptr<EzspEvent> evt = std::make_shared<EzspEvent>(EVT_ERROR, p_status->status, ef->network_index());
                evt->set_frame_id((EId)id);
                add_event(evt);
            }
            else{
                switch(id){
                    case EId::ID_stackStatusHandler:
                    {
                        if(p_status->status == EmberStatus::EMBER_NETWORK_UP || p_status->status == EmberStatus::EMBER_NETWORK_DOWN){
                            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " EVT NET_STATUS " + std::to_string((uint16_t)id));
                            add_event(std::make_shared<EzspEvent>(EVT_NET_STATUS, p_status->status, ef->network_index()));
                        }

                        if(p_status->status == EmberStatus::EMBER_NETWORK_UP){
                            getCurrentSecurityState();
                        }
                    }
                    break;
                    case EId::ID_setInitialSecurityState:
                    {
                        if(is_coordinator()){
                            formNetwork();
                        }
                    }
                    break;
                    case EId::ID_formNetwork:
                    break;
                    case EId::ID_becomeTrustCenter:
                    {
                        become_trust_center(true);
                        add_event(std::make_shared<EzspEvent>(EVT_TRUST_CENTER));
                    }
                    break;
                    case EId::ID_broadcastNextNetworkKey:
                    {
                        broadcastNetworkKeySwitch();
                    }
                    break;
                }
            }
        }
        break;
        /**
         * uint8_t value only
         */
        case EId::ID_neighborCount:
        case EId::ID_getExtendedTimeout:
        {
            auto p_8t_value = ef->load<zb_ezsp::uint8t_value>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_8t_value->to_string());
        }
        break;

        /**
        * Group no data on responses
        */
        case EId::ID_noCallbacks:
        case EId::ID_setExtendedTimeout:
        {
            ef->load<zb_ezsp::no_params>(efr_raw->data(), efr_raw->len());
            notify((EId)id, std::string());
        }
        break;
        /**
         * Reports that a network was found as a result of a prior call to startScan. Gives the network parameters useful for deciding which network to join.
         */
        case EId::ID_networkFoundHandler:
        {
            if(is_debug()){
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " networkFoundHandler: " + zb_ezsp::Conv::print_buff(efr_raw->data(), efr_raw->len()));
            }

            auto p_network = ef->load<zb_ezsp::networkFoundHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_network->to_string());
        }
        break;
        case EId::ID_childJoinHandler:
        {
            auto p_child = ef->load<zb_ezsp::childJoinHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_child->to_string());

            /**
             * Save child information of child Joined and delete if un-Joined
             */
            if(p_child->joining > 0 ){
                _childs->add_child(p_child);

                if(p_child->childType == EmberNodeType::EMBER_SLEEPY_END_DEVICE){
                    //TODO: Later
                    //getExtendedTimeout(p_child->childId);
                    setExtendedTimeout(p_child->childEui64, true);
                    //getExtendedTimeout(p_child->childId);
                }
            }
            else
            {
                //_childs->del_child(p_child->childId);
                _childs->set_child_join_status(p_child->childEui64, false);
            }

        }
        break;
        case ID_stackTokenChangedHandler:
        {
            auto p_token =  ef->load<zb_ezsp::stackTokenChangedHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_token->to_string());
        }
        break;
        case ID_trustCenterJoinHandler:
        {
            auto p_trust =  ef->load<zb_ezsp::trustCenterJoinHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_trust->to_string());

            auto child = _childs->add_child(p_trust);
            if(child){
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " child: " + Conv::to_string(child->childId) + " EUI64: " + Conv::to_string(child->childEui64) + " Status: " + std::to_string(p_trust->status));

                /**
                 * Start procedure for Address Table
                 */
                const EmberNodeId active_child = _childs->get_next_for_address_table();
                if(active_child == childs::Child::NoChild){
                    _childs->set_active_child(child->childId);
                    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " set actice child: " + Conv::to_string(child->childId));
                }

                if(active_child == child->childId){
                    //Find child in address Table
                    lookupEui64ByNodeId(child->childId);
                }

                if(p_trust->status == EmberDeviceUpdate::EMBER_STANDARD_SECURITY_UNSECURED_JOIN){
                    //sendApsTransportKey(child);
                }
            }


        }
        break;
        case EId::ID_lookupEui64ByNodeId:
        {
            auto p_trust =  ef->load<zb_ezsp::lookupEui64ByNodeId>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_trust->to_string());

        }
        break;
        case EId::ID_getEui64:
        {
            auto p_eui64 = ef->load<zb_ezsp::Eui64>(efr_raw->data(), efr_raw->len());
            memcpy(this->_eui64, p_eui64->eui64, sizeof(EmberEUI64));
            notify((EId)id, Conv::to_string(p_eui64->eui64));

        }
        break;
        case EId::ID_getNodeId:
        {
            auto p_nodeid = ef->load<zb_ezsp::NodeId>(efr_raw->data(), efr_raw->len());
            this->_nodeId = p_nodeid->nodeId;
            notify((EId)id, Conv::to_string(this->_nodeId));
        }
        break;
        case EId::ID_networkState:
        {
            auto p_netstate = ef->load<zb_ezsp::networkState>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_netstate->to_string());
        }
        break;
        case EId::ID_getNetworkParameters:
        {
            auto p_netstate = ef->load<zb_ezsp::getNetworkParameters_resp>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_netstate->to_string());
        }
        break;
        case EId::ID_getParentChildParameters:
        {
            auto p_netstate = ef->load<zb_ezsp::getParentChildParameters>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_netstate->to_string());
        }
        break;
        case EId::ID_scanCompleteHandler:
        {
            auto p_handler = ef->load<zb_ezsp::scanCompleteHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_handler->to_string());
        }
        break;
        case EId::ID_messageSentHandler:
        {
            auto p_sentMsg = ef->load<zb_ezsp::messageSentHandler>(efr_raw->data(), efr_raw->len());
            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " messageSentHandler TAG: " + std::to_string((uint16_t)p_sentMsg->messageTag));

            auto child = _childs->get_child_by_child_id(p_sentMsg->indexOrDestination);
            if(child){
                child->in_progress(false);
                child->set_squence(p_sentMsg->apsFrame.sequence+1);
            }


            notify((EId)id, p_sentMsg->to_string());
        }
        break;
        case EId::ID_getBinding:
        {
            auto p_handler = ef->load<zb_ezsp::getBinding>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_handler->to_string());
        }
        break;
        case EId::ID_incomingMessageHandler:
        {
            auto p_inMsg = ef->load<zb_ezsp::incomingMessageHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_inMsg->to_string());
        }
        break;
        case EId::ID_energyScanResultHandler:
        {
            auto p_channel = ef->load<zb_ezsp::energyScanResultHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_channel->to_string());
        }
        break;
        case EId::ID_getChildData:
        {
            auto p_cldData = ef->load<zb_ezsp::getChildData>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_cldData->to_string());
        }
        break;
        case EId::ID_getConfigurationValue:
        {
            auto p_conf = ef->load<zb_ezsp::configid_get_resp>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_conf->to_string());
        }
        break;
        case EId::ID_getCurrentSecurityState:
        {
            auto p_conf = ef->load<zb_ezsp::getCurrentSecurityState>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_conf->to_string());
        }
        break;
        case EId::ID_getKey:
        {
            auto p_key = ef->load<zb_ezsp::getKey>(efr_raw->data(), efr_raw->len());
            if(p_key->status == EmberStatus::EMBER_SUCCESS){
                if(p_key->keyStruct.type == EmberKeyTypeEnum::EMBER_CURRENT_NETWORK_KEY){
                    _key_network.copy_data(p_key->keyStruct);
                }
                else if(p_key->keyStruct.type == EmberKeyTypeEnum::EMBER_TRUST_CENTER_LINK_KEY){
                    _key_trust_center_link.copy_data(p_key->keyStruct);
                }
            }

            notify((EId)id, p_key->to_string());

        }
        break;
        case EId::ID_sendUnicast:
        {
            auto p_conf = ef->load<zb_ezsp::sendUnicast_resp>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_conf->to_string());
        }
        break;
        case EId::ID_incomingRouteErrorHandler:
        {
            auto p_sendUnicast = ef->load<zb_ezsp::incomingRouteErrorHandler>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_sendUnicast->to_string());
        }
        break;
        /**
         * Data array based responses
         */
        case EId::ID_Echo:
        case EId::ID_getMfgToken:
        {
            auto p_echo = ef->load<zb_ezsp::data_array>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_echo->to_string());
        }
        break;
        case EId::ID_getValue:
        {
            auto p_status = ef->load<zb_ezsp::value_get_resp>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_status->to_string());
        }
        break;
        /**
         * EZSP status only
         */
        case EId::ID_setValue:
        case EId::ID_setConfigurationValue:
        {
            auto p_status = ef->load<zb_ezsp::ezsp_status>(efr_raw->data(), efr_raw->len());
            notify((EId)id, p_status->to_string());
        }
        break;

        default:
            notify((EId)id, std::string("Not supported yet"));
    }

    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " --- EFRAME --" + ef->to_string());
}

}

