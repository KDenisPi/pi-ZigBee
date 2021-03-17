/**
 * config.h
 *
 *  Configuration parameters
 *
 *  Created on: Mar 15, 2021
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_CONFIG_H_
#define PI_ZIGBEE_LIB_CONFIF_H_

#include <string>

namespace zb_ezsp {

class Config {
public:
    Config() {}
    ~Config() {}

    const std::string ver() const {
        return _ver;
    }

    std::string _ver;
};

} //zb_ezsp

#endif