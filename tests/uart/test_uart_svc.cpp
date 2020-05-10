#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "uart.h"
#include "ezsp.h"

using namespace std;


/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;
    uint8_t w_buff[140], r_buff[140];
    zb_ezsp::ver_req ver;
    ver._ver = 4;

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);
    sleep(2);

    std::cout << "Start worker" << std::endl;
    uart->start();
    sleep(2);

    std::cout << "Activate processing" << std::endl;
    uart->set_activate(true);
    sleep(15);

    std::cout << "Stop worker. Release object" << std::endl;
    uart.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}