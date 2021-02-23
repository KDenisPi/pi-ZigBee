/**
 * zdp.cpp
 *
 *  ZigBee Device Profile
 *
 * The Device Profile supports four key inter-device communication functions within the ZigBee protocol.
 * These functions are explained in the following sections:
 *  • Device and Service Discovery Overview
 *  • End Device Bind Overview
 *  • Bind and Unbind Overview
 *  • Binding Table Management Overview
 *  • Network Management Overview
 *
 *  Created on: Feb 18, 2021
 *      Author: Denis Kudia
 */

#include <memory>

#include "ezsp_childs.h"
#include "zdp.h"

namespace zb_ezsp {
namespace zdp {

/**
 * Effect on Receipt
 * Upon receipt, the Remote Device shall use the IEEEAddr in the message to find a match with any other
 * IEEE address held in the Remote Device. If a match is detected, the Remote Device shall update the
 * nwkAddressMap attribute of the NIB with the updated NWKAddr corresponding to the IEEEAddr received.
 *
 * The Remote Device shall also use the NWKAddr in the message to find a match with any other 16-bit
 * NWK address held in the Remote Device, even if the IEEEAddr field in the message carries the value of
 * 0xffffffffffffffff. If a match is detected for a device with an IEEE address other than that indicated in the
 * IEEEAddr field received, then this entry shall be marked as not having a known valid 16-bit NWK address.
 */
void Device_Annce::process(std::shared_ptr<childs::Childs>& childs){
    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + to_string());
    auto child = childs->get_child_obj(_IeeeAddr);

    if(child){  //Part 1
        child->macCapability.set(_Capability);
        if(child->nwkAddr != _NwkAddr)
            child->nwkAddr = _NwkAddr;
    }
    else{
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Not found " + Conv::to_string(_IeeeAddr));
    }

    //Part 2
    childs->invalidate(_IeeeAddr, _NwkAddr);

}

}//zdp
}//zb_ezsp