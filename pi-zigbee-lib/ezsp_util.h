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

    static size_t put(uint8_t* buff, size_t pos, const EmberEUI64 val){
        return put(buff, pos, val, sizeof(EmberEUI64), sizeof(EmberEUI64));
    }

    static void copy(EmberEUI64& dest, const EmberEUI64& src){
        memcpy(dest, src, sizeof(EmberEUI64));
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
        size_t rlen = (len<=max_len ? len : max_len);
        memcpy(res, &buff[pos], rlen);
        pos += rlen;
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

        return (EId)get_short(buff, pos);
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
    static const std::string print_buff(const std::uint8_t* buff, const size_t len, bool alnum = false) {
        std::string result;
        char dec[10];
        for(int i=0; i<len; i++){
            int aln = isalnum(buff[i]);
            if(alnum && aln)
                sprintf(dec, "%c ", buff[i]);
            else
                sprintf(dec, "0x%02X ", buff[i]);
            result += dec;
        }
        return result;
    }

    static const std::string Eui64_to_string(const EmberEUI64& eui64){
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

    static const std::string KeyData_to_string(const EmberKeyData& keyData){
        char buff[128];
        std::sprintf(buff, "Key:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
        keyData[0],
        keyData[1],
        keyData[2],
        keyData[3],
        keyData[4],
        keyData[5],
        keyData[6],
        keyData[7],
        keyData[8],
        keyData[9],
        keyData[10],
        keyData[11],
        keyData[12],
        keyData[13],
        keyData[14],
        keyData[15]
        );
        return std::string(buff);
    }

    /**
     *
     */
    static const std::string to_string(const uint16_t ui16){
        char buff[10];
        std::sprintf(buff, "0x%04X", ui16);
        return std::string(buff);
    }

    static const std::string to_string(const EmberEUI64& eui64){
        return Eui64_to_string(eui64);
    }

    static const uint64_t eui642u64(const EmberEUI64& eui64){
        uint64_t rs = 0L;
        auto cnv = [&](uint8_t i)->uint64_t {return ((uint64_t)eui64[i])<<(8*i);};
        rs = cnv(0)|cnv(1)|cnv(2)|cnv(3)|cnv(4)|cnv(5)|cnv(6)|cnv(7);
        return rs;
    }

    static void u642eui64(const uint64_t ui64, EmberEUI64& eue64){
        auto cnv = [&](uint8_t i)->uint8_t { return ((ui64>>(8*i))& 0xFF);};
        for(int i = 0; i < 8; i++) eue64[i] = cnv(i);
    }

    static void clear(EmberEUI64& eui64){
        memset(eui64, 0x00, sizeof(EmberEUI64));
    }

    template<typename T>
    static const bool is_empty(T& eui64){
        for(int i = 0; i < sizeof(T); i++){
            if(eui64[i] > 0x00) return false;
        }
        return true;
    }

    static void copy_ext_pan(const ExtendedPanId& src, ExtendedPanId& dst){
        memcpy(dst, src, sizeof(ExtendedPanId));
    }

};

}
#endif