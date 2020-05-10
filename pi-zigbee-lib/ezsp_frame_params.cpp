/**
 * ezsp_frame_params.cpp
 *
 *  EZSP Frames. Parameters
 *
 *  Created on: May 6, 2020
 *      Author: Denis Kudia
 */

#include <stddef.h>
#include <assert.h>

#include "ezsp_frame_params.h"
#include "ezsp_frame.h"

namespace zb_ezsp {

/**
 * Version
 */
template<>
size_t EFrame<ver_req>::put(ver_req& param, uint8_t* buff, size_t pos){
    return Conv::put(buff, pos, param._ver);
}

template<>
void EFrame<ver_resp>::get(ver_resp& param, const uint8_t* buff, size_t pos){
    param._ver = Conv::get_byte(buff, pos);
    param._stackType = Conv::get_byte(buff, pos);
    param._stackVersion = Conv::get_short(buff, pos);
}

/**
 * Echo
 */
template<>
size_t EFrame<zb_ezsp::echo>::put(zb_ezsp::echo& param, uint8_t* buff, size_t pos){
    assert(param.dataLength<=sizeof(param.data));

    pos = Conv::put(buff, pos, param.dataLength);
    for(int i=0; i<param.dataLength; i++)
        pos = Conv::put(buff, pos, param.data[i]);

    return pos;
}

template<>
void EFrame<zb_ezsp::echo>::get(zb_ezsp::echo& param, const uint8_t* buff, size_t pos){
    param.dataLength = Conv::get_byte(buff, pos);
    assert(param.dataLength<=sizeof(param.data));

    for(int i=0; i<param.dataLength && i < sizeof(param.data); i++)
        param.data[i] = Conv::get_byte(buff, pos);
}

template<>
const size_t EFrame<zb_ezsp::echo>::param_length(const zb_ezsp::echo& param) const {
    return (param.dataLength+1);
}


}