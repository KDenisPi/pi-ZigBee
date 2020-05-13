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

#include "ezsp_defs.h"

namespace zb_ezsp {

class Conv
{
public:
    /**
     *
     */
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

    static size_t get(const uint8_t* buff, size_t& pos, uint8_t& res){
        res = buff[pos++];
        return pos;
    }

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
};

}
#endif