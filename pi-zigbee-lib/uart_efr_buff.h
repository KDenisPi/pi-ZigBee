/**
 * uart_efr_buff.h
 *
 * UART support. Object with EZSP frame content
 *
 *  Created on: May 12, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_UART_EBUFF_H_
#define PI_ZIGBEE_LIB_UART_EBUFF_H_

#include <cstdint>
#include <stddef.h>
#include <memory>
#include <cstring>
#include <array>

namespace zb_uart {

class EFData{
public:
    EFData(const size_t len, const uint8_t* buff){
        _len = len;
        _buff = std::make_shared<std::array<uint8_t, 132>>();
        memcpy(_buff->data(), buff, len);
    }

    EFData(EFData&& other){
        this->_len = other._len;
        this->_buff = std::move(other._buff);
    }

    EFData& operator=(EFData&& other) {
        this->_len = other._len;
        this->_buff = std::move(other._buff);
        return *this;
    }

    EFData& operator=(const EFData&) = delete;
    EFData(const EFData&) = delete;

    const size_t len() const {
        return _len;
    }

    const uint8_t* data() {
        return _buff->data();
    }

    size_t _len;
    std::shared_ptr<std::array<uint8_t, 132>> _buff;
};

}

#endif
