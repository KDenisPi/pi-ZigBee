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

const bool compare_buffers(const uint8_t* buf_one, const size_t len_one, const uint8_t* buf_two, const size_t len_two){
    if(len_one != len_two) return false;

    for(int i=0; i<len_one; i++){
        if(buf_one[i] != buf_two[i]) return false;
    }
    return true;
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

    /**
     * Version Frame
     */

    std::cout << "*** Version frame ***" << std::endl;
    std::shared_ptr<zb_ezsp::EFrame> ef_ver = std::make_shared<zb_ezsp::EFrame>(zb_ezsp::EId::ID_version);
    std::cout << ef_ver->to_string() << std::endl;

    memset(buffer, 0x00, sizeof(buffer));
    size_t len = ef_ver->put<zb_ezsp::ver_req>(buffer, 0, ver);
    std::cout << print_buff(buffer, len) << std::endl;

    std::shared_ptr<zb_uart::UFrame> fr = uart->compose_data(buffer, len);
    std::cout << fr->to_string() << std::endl;

    memset(buffer, 0x00, sizeof(buffer));
    size_t wr_len = uart->encode(fr, buffer, sizeof(buffer));
    std::cout << print_buff(buffer, wr_len) << std::endl;

    /**
     * Version response frame
     */
    std::shared_ptr<zb_ezsp::EFrame> ef_ver_resp = std::make_shared<zb_ezsp::EFrame>();
    auto params = ef_ver_resp->load<zb_ezsp::ver_resp>(src_buff, sizeof(src_buff));

    std::cout << ef_ver_resp->to_string() << std::endl;


    /**
     * Echo frame
     */

    std::cout << "*** Echo frame ***" << std::endl;

    zb_ezsp::echo echo_;
    echo_.dataLength = 10;
    uint8_t ch = 'A';
    for(int i=0; i<echo_.dataLength; i++)
    echo_.data[i] = ch+i;

    uint8_t buffer_in[128], buffer_out[128];
    std::shared_ptr<zb_ezsp::EFrame> ef_echo = std::make_shared<zb_ezsp::EFrame>(zb_ezsp::EId::ID_Echo);

    std::cout << ef_echo->to_string() << std::endl;
    memset(buffer_in, 0x00, sizeof(buffer));
    size_t len_in = ef_echo->put<zb_ezsp::echo>(buffer_in, 0, echo_);
    std::cout << print_buff(buffer_in, len_in) << std::endl;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}