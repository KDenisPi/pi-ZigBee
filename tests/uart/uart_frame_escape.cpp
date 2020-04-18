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

struct FrameInfo {
    zb_uart::ftype ft;
    size_t data_len;
    uint8_t data[133];
};

FrameInfo frames[] = {
    {zb_uart::ftype::RST, 0, {0xC0, 0x38, 0xBC, 0x7E}},
    {zb_uart::ftype::RSTACK, 2, {0xC1, 0x02, 0x02, 0x9B, 0x7B, 0x7E}}
};



/**
*
*/
int main (int argc, char* argv[])
{
    bool success = true;
    std::uint8_t buffer[256];
    size_t esc_len = 0;
    zb_uart::uCRC crc;


    logger::log_init("/var/log/logs/uart_test");

    /**
     * Simple encoding/decoding
     */

    //Create DATA frame
    std::shared_ptr<zb_uart::ZBUart> uart = std::make_shared<zb_uart::ZBUart>(true);

    for(int i=0; i<2; i++){
        memset(buffer, 0x00, sizeof(buffer));

        std::cout << "Test Frame type: " << zb_uart::UFrame::ftype2str(frames[i].ft) << std::endl;
        std::cout << "Original: " << print_buff(frames[i].data, frames[i].data_len+4) << std::endl;

        //decode frame from byte array
        std::shared_ptr<zb_uart::UFrame> fr = uart->parse(frames[i].data, frames[i].data_len+4);
        if(fr){
            std::cout << fr->to_string() << std::endl;
            //encode frame back to byte array
            size_t fr_len = uart->encode(fr, buffer, sizeof(buffer), false);
            success = compare_buffers(frames[i].data, frames[i].data_len+4, buffer, fr_len);
            if(!success){
                std::cout << "Encoded: " << print_buff(buffer, fr_len) << std::endl;
            }
        }
        else{
            std::cout << zb_uart::UFrame::ftype2str(frames[i].ft) << " Decode error" << std::endl;
            success = false;
        }

        fr.reset();
    }

    std::cout << "Finished " << success << std::endl;
    std::exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}