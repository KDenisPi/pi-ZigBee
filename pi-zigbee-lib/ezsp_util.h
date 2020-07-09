/**
 * ezsp_util.h
 *
 *  EZSP Utils
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_UTIL_H_
#define PI_ZIGBEE_LIB_EZSP_UTIL_H_

#include <cstdint>
#include <stddef.h>
#include <assert.h>

#include "ezsp_defs.h"

namespace zb_ezsp {

class Conv
{
public:
    /**
     *
     */
    static const size_t put(uint8_t* buff, size_t pos, const uint64_t val){
        pos = put(buff, pos, Low(val));
        pos = put(buff, pos, High(val));
        return pos;
    }

    static const size_t put(uint8_t* buff, size_t pos, const uint32_t val){
        pos = put(buff, pos, Low(val));
        pos = put(buff, pos, High(val));
        return pos;
    }

    static size_t put(uint8_t* buff, size_t pos, const uint16_t val){
        buff[pos++] = Low(val);
        buff[pos++] = High(val);
        return pos;
    }

    static size_t put(uint8_t* buff, size_t pos, const uint8_t val){
        buff[pos++] = val;
        return pos;
    }

    /**
     * Put byte array
     */
    static size_t put(uint8_t* buff, size_t& pos, const uint8_t* data, const size_t len, const size_t max_len){
        assert(len<=max_len);
        for(int i=0; i<len && i<max_len; i++)
            buff[pos++] = data[i];
        return pos;
    }


    static size_t get(const uint8_t* buff, size_t& pos, uint8_t& res){
        res = buff[pos++];
        return pos;
    }

    static size_t get(const uint8_t* buff, size_t& pos, int8_t& res){
        res = buff[pos++];
        return pos;
    }

    /**
     * Get byte array
     */
    static size_t get(const uint8_t* buff, size_t& pos, uint8_t* res, const size_t len, const size_t max_len){
        assert(len<=max_len);
        for(int i=0; i<len && i<max_len; i++)
            res[i] = buff[pos++];
        return pos;
    }

/*
    static size_t get(const uint8_t* buff, size_t& pos, uint8_t& res[8], const size_t len, const size_t max_len){
        assert(len<=max_len);
        for(int i=0; i<len && i<max_len; i++)
            res[i] = buff[pos++];
        return pos;
    }
*/
    static size_t get(const uint8_t* buff, size_t& pos, uint16_t& res){
        res = (buff[pos+1] << 8);
        res |= buff[pos];
        pos += sizeof(uint16_t);
        return pos;
    }

    static size_t get(const uint8_t* buff, size_t& pos, uint32_t& res){
        uint32_t low = get_short(buff, pos);
        uint32_t high = (get_short(buff, pos) << 16);
        res = (high|low);
        return pos;
    }

    static size_t get(const uint8_t* buff, size_t& pos, uint64_t& res){
        uint64_t low = get_word(buff, pos);
        uint64_t high = ((uint64_t)get_word(buff, pos) << 32);
        res = (high|low);
        return pos;
    }

    static EId get_id(const uint8_t* buff, size_t& pos){
        if(sizeof(EId)==1)
            return (EId)get_byte(buff, pos);

        return (EId)get_word(buff, pos);
    }

    static uint8_t get_byte(const uint8_t* buff, size_t& pos){
        return buff[pos++];
    }

    static uint16_t get_short(const uint8_t* buff, size_t& pos){
        uint16_t res = (buff[pos+1] << 8);
        res |= buff[pos];
        pos += sizeof(uint16_t);
        return res;
    }

    static uint32_t get_word(const uint8_t* buff, size_t& pos){
        uint32_t low = get_short(buff, pos);
        uint32_t high = (get_short(buff, pos) << 16);
        return (high|low);
    }

    static uint64_t get_dword(const uint8_t* buff, size_t& pos){
        uint64_t low = get_word(buff, pos);
        uint64_t high = ((uint64_t)get_word(buff, pos) << 32);
        return (high|low);
    }

    template<typename T>
    static size_t get_byte(const uint8_t* buff, size_t& pos, T& res){
        uint8_t value;
        pos = Conv::get(buff, pos, value);
        res = (T)value;
        return pos;
    }

    template<typename T>
    static size_t get_word(const uint8_t* buff, size_t& pos, T& res){
        uint16_t value;
        pos = Conv::get(buff, pos, value);
        res = (T)value;
        return pos;
    }


    /**
     * little endian utils
     */
    constexpr static const uint8_t Low(uint16_t val){
        return (val&0x00FF);
    }

    constexpr static const uint8_t High(uint16_t val){
        return (val>>8);
    }

    constexpr static const uint16_t Low(uint32_t val){
        return (val&0xFFFF);
    }

    constexpr static const uint16_t High(uint32_t val){
        return (val>>16);
    }

    constexpr static const uint16_t Low(uint64_t val){
        return (val&0xFFFFFFFF);
    }

    constexpr static const uint16_t High(uint64_t val){
        return (val>>32);
    }

    /**
     * Print buffer. Debug purposes only
     */
    static const std::string print_buff(const std::uint8_t* buff, const size_t len) {
        std::string result;
        char dec[10];
        for(int i=0; i<len; i++){
            sprintf(dec, "0x%02X ", buff[i]);
            result += dec;
        }
        return result;
    }

    static const std::string Eui64_to_string(const EmberEUI64 eui64){
        char buff[128];
        std::sprintf(buff, "EUI64:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
        eui64[0],
        eui64[1],
        eui64[2],
        eui64[3],
        eui64[4],
        eui64[5],
        eui64[6],
        eui64[7]
        );
        return std::string(buff);
    }

    /**
     *
     */
    static const std::string to_string(const uint16_t ui16){
        char buff[10];
        std::sprintf(buff, "%04X", ui16);
        return std::string(buff);
    }
};

}
#endif