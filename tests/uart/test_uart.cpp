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
    uint8_t w_buff[140], r_buff[140];
    zb_ezsp::ver_req ver;
    ver._ver = 4;

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_ezsp::EFrame<zb_ezsp::ver_req>> ef_ver = std::make_shared<zb_ezsp::EFrame<zb_ezsp::ver_req>>(zb_ezsp::EId::ID_version, ver);
    ef_ver->set_seq(1);
    std::cout << ef_ver->to_string() << std::endl;

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);
    if(uart->connect("/dev/ttyUSB1", 57600)){
        if(uart->init_device(3)){

            memset(w_buff, 0x00, sizeof(w_buff));
            size_t wr_len = ef_ver->put(w_buff, 0);
            std::cout << print_buff(w_buff, wr_len) << std::endl;

            std::shared_ptr<zb_uart::UFrame> fr = uart->compose_data(w_buff, wr_len);
            memset(w_buff, 0x00, sizeof(w_buff));
            wr_len = uart->encode(fr, w_buff, sizeof(w_buff), true);
            std::cout << "For send: " << print_buff(w_buff, wr_len) << std::endl;
            std::cout << fr->to_string() << std::endl;

            int res = uart->write_data(w_buff, wr_len);
            std::cout << "Write: " << res << std::endl;

            memset(r_buff, 0x00, sizeof(r_buff));
            size_t rd_len = sizeof(r_buff);
            res = uart->read_data(r_buff, rd_len);
            std::cout << "Read: " << res << " Bytes: " << rd_len << std::endl;

            std::cout << print_buff(r_buff, rd_len) << std::endl;
            std::shared_ptr<zb_uart::UFrame> fr_rsv = uart->parse(r_buff, rd_len, true);
            if(fr_rsv){
                std::cout << fr_rsv->to_string() << std::endl;
                std::shared_ptr<zb_ezsp::EFrame<zb_ezsp::ver_resp>> ef_ver_resp = std::make_shared<zb_ezsp::EFrame<zb_ezsp::ver_resp>>();
                ef_ver_resp->load(fr_rsv->data(), fr_rsv->data_len());
                std::cout << ef_ver_resp->to_string() << std::endl;
            }

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