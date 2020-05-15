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
size_t EFrame::put_param(const ver_req& param, uint8_t* buff, size_t pos){
    return Conv::put(buff, pos, param._ver);
}

void EFrame::get_param(ver_resp& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param._ver);
    pos = Conv::get(buff, pos, param._stackType);
    pos = Conv::get(buff, pos, param._stackVersion);
}

/**
 * Echo
 */
size_t EFrame::put_param(const zb_ezsp::echo& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.dataLength);
    for(int i=0; i<param.dataLength; i++)
        pos = Conv::put(buff, pos, param.data[i]);

    return pos;
}

void EFrame::get_param(zb_ezsp::echo& param, const uint8_t* buff, size_t pos){
    pos = Conv::get(buff, pos, param.dataLength);
    assert(param.dataLength<=sizeof(param.data));

    for(int i=0; i<param.dataLength && i < sizeof(param.data); i++)
        pos = Conv::get(buff, pos, param.data[i]);
}

/*
const size_t EFrame::param_length(const zb_ezsp::echo& param) const {
    return sizeof(zb_ezsp::echo); //TEMP (param.dataLength+1);
}
*/

}