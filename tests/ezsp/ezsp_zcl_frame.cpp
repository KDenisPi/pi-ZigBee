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
    uint8_t buff[140];
    size_t len = 0;

    logger::log_init("/var/log/logs/zigbee_log");

    zb_ezsp::zcl::ZclFrame fzcl(zb_ezsp::zcl::GeneralCmd::DiscoverAttr);
    len = fzcl.put(buff, sizeof(buff));

    std::string zcl_data = zb_ezsp::Conv::print_buff(buff, len);
    std::cout << "Easy: " << len << " " << zcl_data << std::endl;
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ZCL " + zcl_data);

    fzcl.MnfSpec();
    len = fzcl.put(buff, sizeof(buff));
    zcl_data = zb_ezsp::Conv::print_buff(buff, len);
    std::cout << "Mnfc: " << len << " " << zcl_data << std::endl;
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " ZCL " + zcl_data);

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}