#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "ezsp.h"
#include "ezsp_db_json.h"

using namespace std;


/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;

    zb_ezsp::Config cfg;
    zb_ezsp::net_array nets = std::make_shared<std::array<zb_ezsp::net_info, 20>>();
    zb_ezsp::ChildsPtr childs = std::make_shared<zb_ezsp::childs::Childs>();

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_ezsp::EzspDbJson> conf = std::make_shared<zb_ezsp::EzspDbJson>("/home/deniskudia/sources/pi-ZigBee/tests/ezsp/config.json");
    success = conf->load(cfg);
    std::cout << "Load config result " << success << std::endl;

    if(success){
        success = conf->load_networks(nets);
        std::cout << "Load networks result " << success << std::endl;

        if(success){
            success = conf->load_childs(childs);
            std::cout << "Load childs result " << success << std::endl;

            if(success){
                success = conf->save(cfg, nets, childs);
            }
        }
    }

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}