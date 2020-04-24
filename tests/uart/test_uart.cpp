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
    bool success = false;
    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);
    if(uart->connect("/dev/ttyUSB1", 57600)){
        if(uart->init_device(3)){
            success = true;
        }
        else{
            std::cout << "Device initialization failed" << std::endl;
        }
    }
    else{
        std::cout << "Connection failed" << std::endl;
    }

    uart.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}