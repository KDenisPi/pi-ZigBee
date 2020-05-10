/**
 * crc16.h
 *
 *  UART Frames. CRC16 implementation
 *
 *  Created on: Apr 2, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_UART_CRC16_H_
#define PI_ZIGBEE_LIB_UART_CRC16_H_

#include <cstdint>
#include "stddef.h"

namespace zb_uart {

/**
 * CRC16 (CRC-CCIT)
 *
 * From linux/crc-ccitt.h
 *
 */
class CRC16 {
public:

/*
 * This mysterious table is just the CRC of each possible byte. It can be
 * computed using the standard bit-at-a-time methods. The polynomial can
 * be seen in entry 128, 0x8408. This corresponds to x^0 + x^5 + x^12.
 * Add the implicit x^16, and you have the standard CRC-CCITT.
 */
    static uint16_t const crc_ccitt_table[256];
/*
 * Similar table to calculate CRC16 variant known as CRC-CCITT-FALSE
 * Reflected bits order, does not augment final value.
 */
    static uint16_t const crc_ccitt_false_table[256];

    static inline uint16_t crc_ccitt_byte(uint16_t crc, const uint8_t c)
    {
        return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
    }

    static inline uint16_t crc_ccitt_false_byte(uint16_t crc, const uint8_t c)
    {
        return (crc << 8) ^ crc_ccitt_false_table[(crc >> 8) ^ c];
    }

    /**
     *	crc_ccitt - recompute the CRC (CRC-CCITT variant) for the data
    *	buffer
    *	@crc: previous CRC value
    *	@buffer: data pointer
    *	@len: number of bytes in the buffer
    */
    static inline uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len)
    {
        while (len--)
            crc = crc_ccitt_byte(crc, *buffer++);
        return crc;
    }

    static inline uint16_t crc_ccitt(uint8_t const *buffer, size_t len){
        return crc_ccitt_false(0xFFFF, buffer, len);
    }


    /**
     *	crc_ccitt_false - recompute the CRC (CRC-CCITT-FALSE variant)
    *	for the data buffer
    *	@crc: previous CRC value
    *	@buffer: data pointer
    *	@len: number of bytes in the buffer
    */
    static inline uint16_t crc_ccitt_false(uint16_t crc, uint8_t const *buffer, size_t len)
    {
        while (len--)
            crc = crc_ccitt_false_byte(crc, *buffer++);
        return crc;
    }
};



union uCRC {
    uint16_t crc;
    uint8_t bytes[2];
};


}
#endif