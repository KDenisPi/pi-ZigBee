#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "uart.h"
#include "ezsp.h"

using namespace std;

template<typename T>
static void ezsp_frame_to_send(const std::shared_ptr<zb_uart::ZBUart>& uart, const std::shared_ptr<zb_ezsp::EFrame>& ef);
static void ezsp_frame_test(const std::shared_ptr<zb_uart::ZBUart>& uart, zb_ezsp::EId id);


const std::string print_buff(const std::uint8_t* buff, size_t len) {
    std::string result = "Len: " + std::to_string(len) + "\n";
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

static void ezsp_frame_test(const std::shared_ptr<zb_uart::ZBUart>& uart, zb_ezsp::EId id){
    std::shared_ptr<zb_ezsp::EFrame> ef  = std::make_shared<zb_ezsp::EFrame>(id);
    std::cout << "---- frame ID " << id << " " << zb_ezsp::Ezsp::get_id_name(id) <<std::endl;
    switch(id){
        case zb_ezsp::EId::ID_version:
            ezsp_frame_to_send<zb_ezsp::ver_req>(uart, ef);
            break;
        case zb_ezsp::EId::ID_Echo:
            ezsp_frame_to_send<zb_ezsp::data_array>(uart, ef);
            break;
        case zb_ezsp::EId::ID_unicastNwkKeyUpdate:
            ezsp_frame_to_send<zb_ezsp::unicastNwkKeyUpdate>(uart, ef);
            break;
        default:
            std::cout << "***** Not supported!!! *****" <<std::endl;
    }
}

template<typename T>
static void ezsp_frame_to_send(const std::shared_ptr<zb_uart::ZBUart>& uart, const std::shared_ptr<zb_ezsp::EFrame>& ef){
    uint8_t buffer[128];
    uint8_t wr_buffer[128];
    T data;

    memset(buffer, 0x00, sizeof(buffer));
    size_t len = ef->put<T>(buffer, 0, data);
    std::cout << print_buff(buffer, len) << std::endl;

    std::shared_ptr<zb_uart::UFrame> fr = uart->compose_data(buffer, len);
    std::cout << fr->to_string() << std::endl;

    memset(wr_buffer, 0x00, sizeof(wr_buffer));
    size_t wr_len = uart->encode(fr, wr_buffer, sizeof(wr_buffer));
    std::cout << print_buff(wr_buffer, wr_len) << std::endl;
}

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;

    logger::log_init("/var/log/logs/zigbee_log");

    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);

    /**
     * Version Frame
     */
    //std::cout << "*** Version frame ***" << std::endl;
    //ezsp_frame_test(uart, zb_ezsp::EId::ID_version);

    /**
     * Version response frame
     */
    /*
    std::shared_ptr<zb_ezsp::EFrame> ef_ver_resp = std::make_shared<zb_ezsp::EFrame>();
    auto params = ef_ver_resp->load<zb_ezsp::ver_resp>(src_buff, sizeof(src_buff));
    std::cout << ef_ver_resp->to_string() << std::endl;
    */

    /**
     * Echo frame
     */

    //std::cout << "*** Echo frame ***" << std::endl;
    //ezsp_frame_test(uart, zb_ezsp::EId::ID_Echo);

    std::cout << "*** Echo frame ***" << std::endl;
    ezsp_frame_test(uart, zb_ezsp::EId::ID_unicastNwkKeyUpdate);


    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}