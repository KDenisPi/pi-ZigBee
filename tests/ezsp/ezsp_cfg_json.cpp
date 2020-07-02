#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "ezsp.h"

using namespace std;


/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_ezsp::Ezsp> ezsp = std::make_shared<zb_ezsp::Ezsp>(true, "/home/deniskudia/sources/pi-ZigBee/tests/ezsp/config.json");


    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}