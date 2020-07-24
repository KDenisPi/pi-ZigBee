#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "ezsp.h"

using namespace std;

std::shared_ptr<zb_ezsp::Ezsp> ezsp;

void ezsp_callback(const zb_ezsp::EId id, const std::string info){
    std::cout << "ID:" << std::hex << (uint16_t)id << " " << info << std::endl;
}

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;
    std::string cmd;

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

        std::cout << "process command " << cmd << std::endl;
        if(cmd == "exit"){

            logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + "Exit command");

            ezsp->stop();
            sleep(5);
            break;
        }
        if(cmd == "getvalue"){
            ezsp->getValue(zb_ezsp::EzspValueId::EZSP_VALUE_NODE_SHORT_ID);
        }
        else if(cmd == "getcfg"){
            ezsp->getCinfigurationValue(zb_ezsp::EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT);
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
        else if(cmd == "sendunicast"){
            ezsp->sendUnicast();
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
            ezsp->setBinding();
        }
        else if(cmd == "clearBind"){
            ezsp->clearBindingTable();
        }
        else if(cmd == "help"){
            std::cout << "Commands: exit, getvalue, getcfg, startscana, startscane, stopscan, echo, getnetparams, getsecstate, neighbors, neighborCount, sendunicast, getchild, getDeviceInfo, bind, clearBind" << std::endl;
        }
    }

    std::cout << "Stop worker. Release object" << std::endl;
    ezsp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}