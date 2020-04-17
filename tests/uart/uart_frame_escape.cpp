/**
 * UART Frame encode/decode test
 *
 *  Created on: Apr 7, 2020
 *      Author: Denis Kudia
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <arpa/inet.h>

#include "logger.h"
#include "uart.h"

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
    bool success = true;
    //etalon frame contents
    std::uint8_t rst[4] = {0xC0, 0x38, 0xBC, 0x7E};

    std::uint8_t data_2[4] = {0xC0, 0x00, 0x00, 0x00};
    std::uint8_t buffer[256];
    size_t esc_len = 0;
    zb_uart::uCRC crc;

    logger::log_init("/var/log/logs/uart_test");

    /**
     * Simple encoding/decoding
     */

    //Create DATA frame
    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);

    std::shared_ptr<zb_uart::UFrame> out_rst = uart->compose(zb_uart::ftype::RST);
    std::cout << "RST: " << std::endl << out_rst->to_string() << std::endl;

    size_t fr_len = uart->encode(out_rst, buffer, sizeof(buffer), true);
    std::cout << print_buff(buffer, fr_len) << std::endl;

    success = compare_buffers(rst, 4, buffer, fr_len);

    std::shared_ptr<zb_uart::UFrame> in_ufr = uart->parse(buffer, fr_len);
    if(in_ufr)
        std::cout << "RST: Decoded Frame " << std::endl << in_ufr->to_string() << std::endl;
    else {
        std::cout << "Decode error" << std::endl;
        success = false;
    }

    std::cout << "Finished " << success << std::endl;
    std::exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}