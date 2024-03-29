#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "ezsp.h"

using namespace std;

std::shared_ptr<zb_ezsp::Ezsp> ezsp;

void ezsp_callback(const zb_ezsp::EId id, const std::string info){
    std::cout << logger::get_time_ms() << " ID:" << std::hex << (uint16_t)id << " " << zb_ezsp::Ezsp::get_id_name(id) << " "<< info << std::endl;
}

std::string& trim(std::string& str){
    str.erase(0, str.find_first_not_of(" \n\r\t"));
    str.erase(str.find_last_not_of(" \n\r\t")+1);
    return str;
}

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;
    std::string cmd;
    std::string param;
    std::string value;

    logger::log_init("/var/log/logs/zigbee_log");

    ezsp = std::make_shared<zb_ezsp::Ezsp>(true, "/home/deniskudia/sources/pi-ZigBee/tests/ezsp/config.json");
    ezsp->frame_received = std::bind(&ezsp_callback, std::placeholders::_1, std::placeholders::_2);
    sleep(2);

    std::cout << "Start worker" << std::endl;
    ezsp->start();
    sleep(2);

    while(1){
        std::cout << "command> " << std::endl;
        std::cin >> cmd;

        if(cmd == "getValue"|| cmd == "getKey" || cmd == "setcfg" | cmd == "getcfg"){
            std::cin >> param;
            std::cout << "parameter " << param << std::endl;

            if(cmd == "setcfg"){
                std::cin >> value;
                std::cout << "parameter " << value << std::endl;
            }
        }

        std::cout << "process command " << cmd << std::endl;
        if(cmd == "exit"){
            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + "Exit command");

            ezsp->stop();
            sleep(5);
            break;
        }
        if(cmd == "getValue"){
            uint8_t valId = (uint8_t)std::stoi(param);
            if( valId <= zb_ezsp::EzspValueId::EZSP_VALUE_RF4CE_SUPPORTED_PROFILES_LIST_){
                ezsp->getValue((zb_ezsp::EzspValueId)valId); //zb_ezsp::EzspValueId::EZSP_VALUE_NODE_SHORT_ID);
            }
        }
        else if(cmd == "getcfg"){
            uint8_t confId = (param.empty() ? zb_ezsp::EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT : (uint8_t)std::stoi(param));
            ezsp->getConfigurationValue(confId);
        }
        else if(cmd == "setcfg"){
            uint8_t confId = (param.empty() ? zb_ezsp::EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT : (uint8_t)std::stoi(param));
            uint16_t value_d = (uint16_t)(value.empty() ? 0 : std::stoi(value));
            ezsp->setConfigurationValue(confId, value_d);
        }
        else if(cmd == "startscana"){
            uint8_t channels[1] = {11};
            ezsp->startScan(zb_ezsp::EzspNetworkScanType::EZSP_ACTIVE_SCAN, 0xFF, channels);
        }
        else if(cmd == "startscane"){
            uint8_t channels[1] = {11};
            ezsp->startScan(zb_ezsp::EzspNetworkScanType::EZSP_ENERGY_SCAN, 0xFF, channels);
        }
        else if(cmd == "stopscan"){
            ezsp->stopScan();
        }
        else if(cmd =="echo"){
            ezsp->echo();
        }
        else if(cmd == "getnetparams"){
            ezsp->getNetworkParameters();
        }
        else if(cmd == "getsecstate"){
            ezsp->getCurrentSecurityState();
        }
        else if(cmd == "neighbors"){
            const std::string nlist = ezsp->neighbors();
            std::cout <<  "List of neighbors: " << std::endl << (nlist.empty() ? "Empty" : nlist) << std::endl;
        }
        else if(cmd == "sendZcl"){
            //ezsp->sendZcl();
        }
        else if(cmd == "getchild"){
            ezsp->get_childData();
        }
        else if(cmd == "neighborCount"){
            ezsp->neighborCount();
        }
        else if(cmd == "getDeviceInfo"){
            ezsp->getDeviceInfo();
        }
        else if(cmd == "bind"){
            //ezsp->setBinding();
        }
        else if(cmd == "clearBind"){
            ezsp->clearBindingTable();
        }
        else if(cmd == "clearKeyTable"){
            ezsp->clearKeyTable();
        }
        else if(cmd == "nextNetKey"){
            ezsp->broadcastNextNetworkKey();
        }
        else if(cmd == "help"){
            std::cout << "Commands: exit, startscana, startscane, stopscan, echo, getnetparams,\n getsecstate, neighbors, neighborCount, sendZcl, getchild, getDeviceInfo,\n\
            bind, clearBind, clearKeyTable, nextNetKey\n\
            getKey [Trust|Net|NextNet|AppLink]\n\
            getcfg ConfigurationValueId\n\
            setcfg ConfigurationValueId Value\n\
            getValue ValueID [11 - The node short ID]" << std::endl;
        }
        else if(cmd == "getKey"){
                if(param == "Net" || param.empty()){
                    ezsp->getKey(zb_ezsp::EmberKeyTypeEnum::EMBER_CURRENT_NETWORK_KEY);
                }
                else if(param == "Trust"){
                    ezsp->getKey(zb_ezsp::EmberKeyTypeEnum::EMBER_TRUST_CENTER_LINK_KEY);
                }
                else if(param == "NextNet"){
                    ezsp->getKey(zb_ezsp::EmberKeyTypeEnum::EMBER_NEXT_NETWORK_KEY);
                }
                else if(param == "AppLink"){
                    ezsp->getKey(zb_ezsp::EmberKeyTypeEnum::EMBER_APPLICATION_LINK_KEY);
                }
        }
    }

    std::cout << "Stop worker. Release object" << std::endl;
    ezsp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}