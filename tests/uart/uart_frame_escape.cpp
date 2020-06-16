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
    bool randm;
};

FrameInfo frames[] = {
    {zb_uart::ftype::RST, 0, {0xC0, 0x38, 0xBC, 0x7E}, false},
    {zb_uart::ftype::RSTACK, 2, {0xC1, 0x02, 0x02, 0x9B, 0x7B, 0x7E}, false},
    {zb_uart::ftype::DATA, 4, {0x25, 0x00, 0x00 ,0x00 ,0x02 ,0x1A ,0xAD ,0x7E}, false},
    {zb_uart::ftype::DATA, 4, {0x25 ,0x42 ,0x21 ,0xA8 ,0x56 ,0xA6 ,0x09 ,0x7E}, true},
    {zb_uart::ftype::DATA, 7, {0x53 ,0x00 ,0x80 ,0x00 ,0x02 ,0x02 ,0x11 ,0x30 ,0x63 ,0x16 ,0x7E}, false},
    {zb_uart::ftype::DATA, 7, {0x53 ,0x42 ,0xA1 ,0xA8 ,0x56 ,0x28 ,0x04 ,0xA9 ,0x96 ,0x23 ,0x7E}, true},
    {zb_uart::ftype::ACK, 0, {0x81 ,0x60 ,0x59 ,0x7E}, true},
    {zb_uart::ftype::ACK, 0, {0x8E ,0x91 ,0xB6 ,0x7E}, true},
    {zb_uart::ftype::NAK, 0, {0xA6 ,0x34 ,0xDC ,0x7E}, true},
    {zb_uart::ftype::NAK, 0, {0xAD ,0x85 ,0xB7 ,0x7E}, true}
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

    int tests = sizeof(frames)/sizeof(FrameInfo);
    for(int i=0; i<tests; i++){
        memset(buffer, 0x00, sizeof(buffer));

        std::cout << std::endl << "Test Frame type: " << zb_uart::UFrame::ftype2str(frames[i].ft) << " Random: " << frames[i].randm << std::endl;
        std::cout << "Original: " << print_buff(frames[i].data, frames[i].data_len+4) << std::endl;

        //decode frame from byte array
        std::shared_ptr<zb_uart::UFrame> fr = uart->parse(frames[i].data, frames[i].data_len+4, frames[i].randm);
        if(fr){
            std::cout << fr->to_string() << std::endl;
            //encode frame back to byte array
            size_t fr_len = uart->encode(fr, buffer, sizeof(buffer), frames[i].randm);
            success = compare_buffers(frames[i].data, frames[i].data_len+4, buffer, fr_len);
            if(!success){
                std::cout << "Compare failed: " << print_buff(buffer, fr_len) << std::endl;
            }
            else
            {
                std::cout << "Frame OK" << std::endl;
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