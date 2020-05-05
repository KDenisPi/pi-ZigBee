#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "uart.h"
#include "ezsp.h"

using namespace std;

const std::string print_buff(const std::uint8_t* buff, size_t len) {
    std::string result;
    char dec[10];
    for(int i=0; i<len; i++){
        sprintf(dec, "0x%02X ", buff[i]);
        result += dec;
    }
    return result;
}


/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;
    //uint8_t src_buff[4] = {0x00, 0x00 ,0x00 ,0x02};
    uint8_t src_buff[7] = {0x00, 0x80, 0x00, 0x04, 0x02, 0x10, 0x54};
    uint8_t buffer[128];
    zb_ezsp::ver_req ver;
    ver._ver = 4;

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);

    std::shared_ptr<zb_ezsp::EFrame<zb_ezsp::ver_req>> ef_ver = std::make_shared<zb_ezsp::EFrame<zb_ezsp::ver_req>>(zb_ezsp::EId::ID_version, ver);
    std::cout << ef_ver->to_string() << std::endl;

    memset(buffer, 0x00, sizeof(buffer));
    size_t len = ef_ver->put(buffer, 0);
    std::cout << print_buff(buffer, len) << std::endl;

    std::shared_ptr<zb_uart::UFrame> fr = uart->compose_data(buffer, len);
    std::cout << fr->to_string() << std::endl;

    memset(buffer, 0x00, sizeof(buffer));
    size_t wr_len = uart->encode(fr, buffer, sizeof(buffer), true);
    std::cout << print_buff(buffer, wr_len) << std::endl;

    std::shared_ptr<zb_ezsp::EFrame<zb_ezsp::ver_resp>> ef_ver_resp = std::make_shared<zb_ezsp::EFrame<zb_ezsp::ver_resp>>();
    std::cout <<  "Response length: " << ef_ver_resp->resp_length() << std::endl;
    ef_ver_resp->load(src_buff, sizeof(src_buff));

    std::cout << ef_ver_resp->to_string() << std::endl;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}