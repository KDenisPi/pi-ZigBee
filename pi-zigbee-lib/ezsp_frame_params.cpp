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

size_t EFrame::get_param(ver_resp& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param._ver);
    pos = Conv::get(buff, pos, param._stackType);
    pos = Conv::get(buff, pos, param._stackVersion);
    return pos;
}

/**
 * Echo
 */
size_t EFrame::put_param(const zb_ezsp::echo& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, param.dataLength);
    pos = Conv::put(buff, pos, param.data, param.dataLength, free_len(pos));
    return pos;
}

size_t EFrame::get_param(zb_ezsp::echo& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.dataLength);
    pos = Conv::get(buff, pos, param.data, param.dataLength, sizeof(param.data));
    return pos;
}

/**
 * Request and response without parameters
 */
size_t EFrame::put_param(const zb_ezsp::ezsp_no_params& param, uint8_t* buff, size_t pos){
    return pos;
}

size_t EFrame::get_param(zb_ezsp::ezsp_no_params& param, const uint8_t* buff, size_t& pos){
    return pos;
}

/**
 * Ember status only
 */
size_t EFrame::get_param(zb_ezsp::ezsp_EmberStatus& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.status);
    return pos;
}

/**
 * EZSP status only
 */
size_t EFrame::get_param(zb_ezsp::ezsp_EzspStatus& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.status);
    return pos;
}

/**
 * Start scan and related calls
 */
size_t EFrame::put_param(const zb_ezsp::start_scan& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.scanType);
    pos = Conv::put(buff, pos, param.channelMask);
    pos = Conv::put(buff, pos, param.duration);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::energyScanResultHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.maxRssiValue);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::EmberZigbeeNetwork& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.panId);
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.extendedPanId, sizeof(param.extendedPanId), sizeof(param.extendedPanId));
    pos = Conv::get(buff, pos, param.stackProfile);
    pos = Conv::get(buff, pos, param.nwkUpdateId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::networkFoundHandler& param, const uint8_t* buff, size_t& pos){
    pos = get_param(param.networkFound, buff, pos);
    pos = Conv::get(buff, pos, param.lastHopLqi);
    pos = Conv::get(buff, pos, param.lastHopRssi);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::scanCompleteHandler& param, const uint8_t* buff, size_t& pos){
    pos = Conv::get(buff, pos, param.channel);
    pos = Conv::get(buff, pos, param.status);
    return pos;
}

/**
 * Value
 */
size_t EFrame::put_param(const zb_ezsp::value_set& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.valueId);
    pos = Conv::put(buff, pos, param.valueLength);
    pos = Conv::put(buff, pos, param.value, param.valueLength, free_len(pos));
    return pos;
}

size_t EFrame::put_param(const zb_ezsp::value_get_req& param, uint8_t* buff, size_t pos){
    pos = Conv::put(buff, pos, (uint8_t)param.valueId);
    return pos;
}

size_t EFrame::get_param(zb_ezsp::value_get_resp& param, const uint8_t* buff, size_t& pos){
    param.valueLength = 0;
    pos = Conv::get(buff, pos, param.status);
    if(param.status == EzspStatus::EZSP_SUCCESS){
        pos = Conv::get(buff, pos, param.valueLength);
        pos = Conv::get(buff, pos, param.value, param.valueLength, sizeof(param.value));
    }
    return pos;

}


}