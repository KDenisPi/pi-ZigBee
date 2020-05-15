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

    std::shared_ptr<zb_ezsp::Ezsp> ezsp = std::make_shared<zb_ezsp::Ezsp>(true);
    sleep(2);

    std::cout << "Start worker" << std::endl;
    ezsp->start();
    sleep(2);

    std::cout << "Activate processing" << std::endl;
    ezsp->activate();
    sleep(15);

    std::cout << "Stop worker. Release object" << std::endl;
    ezsp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}