#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include  <cstring>

#include "ezsp_util.h"

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

uint8_t test(uint8_t& ccnt){
    return ccnt++;
}

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = false;
    uint16_t val16, val16_get = 0;
    uint32_t val32, val32_get = 0;
    uint8_t buff[132];

    uint8_t st_cnt = 1;
    uint8_t n_cnt = test(st_cnt);


    val16 = 1500;
    val32 = 0xA0B0C0D0;
    printf("%X Low:%X High:%X\n", val16,  zb_ezsp::Conv::Low(val16), zb_ezsp::Conv::High(val16));
    printf("%X 0:%X 1:%X 2:%X 3:%X\n", val32,  zb_ezsp::Conv::Low(zb_ezsp::Conv::Low(val32)), zb_ezsp::Conv::High(zb_ezsp::Conv::Low(val32)), zb_ezsp::Conv::Low(zb_ezsp::Conv::High(val32)), zb_ezsp::Conv::High(zb_ezsp::Conv::High(val32)));

    size_t pos = 0;
    memset(buff, 0x00, sizeof(buff));
    size_t len = zb_ezsp::Conv::put(buff, pos, val16);
    std::cout << "Len: " << len << std::endl << print_buff(buff, len) << std::endl;

    pos = 0;
    val16_get = zb_ezsp::Conv::get_short(buff, pos);
    printf("Val16:%X Pos:%lu\n", val16_get, pos);

    pos = 0;
    memset(buff, 0x00, sizeof(buff));
    len = zb_ezsp::Conv::put(buff, pos, val32);
    std::cout << "Len: " << len << std::endl << print_buff(buff, len) << std::endl;

    pos = 0;
    val32_get = zb_ezsp::Conv::get_word(buff, pos);
    printf("Val32:%X Pos:%lu\n", val32_get, pos);


    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}