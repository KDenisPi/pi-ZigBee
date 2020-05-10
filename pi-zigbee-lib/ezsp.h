/**
 * ezsp.h
 *
 *  EZSP Implementation
 *
 *  Created on: Apr 27, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_H_
#define PI_ZIGBEE_LIB_EZSP_H_

#include "ezsp_frame.h"

namespace zb_ezsp {

/**
 * EZSP implementation
 */
class Ezsp {
public:
    Ezsp() : _seq(0) {}
    ~Ezsp() {}

    Ezsp(Ezsp&&) = delete;
    Ezsp& operator=(const Ezsp&) = delete;
    Ezsp& operator=(Ezsp&&) = delete;


    /**
     * Sequence number
     */
    const uint8_t seq_next() {
        if(_seq == 0xFF) _seq = 0;
        return _seq++;
    }

    const uint8_t seq() const {
        return _seq;
    }

private:
    uint8_t _seq;
};

}

#endif
