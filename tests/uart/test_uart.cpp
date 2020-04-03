#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "uart.h"

using namespace std;

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = true;
    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>();

    uart->connect("/dev/ttyUSB1", 57600);

    uart.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}