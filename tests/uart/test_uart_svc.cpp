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

    ezsp = std::make_shared<zb_ezsp::Ezsp>(true);
    ezsp->frame_received = std::bind(&ezsp_callback, std::placeholders::_1, std::placeholders::_2);
    sleep(2);

    std::cout << "Start worker" << std::endl;
    ezsp->start();
    sleep(2);

    while(1){
        std::cout << "command> " << std::endl;
        std::cin >> cmd;

        if(cmd == "exit")
            break;
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
        else if(cmd == "formnetwork"){
            ezsp->formNetwork();
        }
        else if(cmd == "permitjoin"){
            ezsp->permitJoining();
        }
        else if(cmd == "leavenetwork"){
            ezsp->leaveNetwork();
        }
        else if(cmd == "getnetparams"){
            ezsp->getNetworkParameters();
        }
        else if(cmd == "initsecstate"){
            ezsp->setInitialSecurityState();
        }
        else if(cmd == "getsecstate"){
            ezsp->getCurrentSecurityState();
        }
        else if(cmd == "help"){
            std::cout << "Commands: exit, getvalue, getcfg, startscana, startscane, stopscan, echo, formnetwork, permitjoin, leavenetwork, getnetparams, initsecstate, getsecstate" << std::endl;
        }
    }

    std::cout << "Stop worker. Release object" << std::endl;
    ezsp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}