/**
 * child.h
 *
 *  Child object.
 *
 *  Created on: Mar 15, 2021
 *      Author: Denis Kudia
 */

#ifndef PI_ZIGBEE_LIB_EZSP_CHILD_H_
#define PI_ZIGBEE_LIB_EZSP_CHILD_H_

#include "logger.h"
#include "ezsp_defs.h"
#include "ezsp_frame_params.h"

namespace zb_ezsp {
namespace childs {

class Child {
public:
    Child() : addressTableIndex(0xFF), sequence(0), send_in_progress(false), joining(false),
    childType(EmberNodeType::EMBER_UNKNOWN_DEVICE), devUpdate(EmberDeviceUpdate::EMBER_UNDEFINED), nwkAddr(NoChildId)
    {

    };

    Child(const std::shared_ptr<zb_ezsp::childJoinHandler> childHnd) : Child() {
        index = childHnd->index;
        joining = childHnd->joining;
        nwkAddr = childHnd->childId;
        childType = childHnd->childType;
        memcpy(ieeeAddr, childHnd->childEui64, sizeof(EmberEUI64));

        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + to_string());
    }

    Child(const std::shared_ptr<zb_ezsp::trustCenterJoinHandler> trustHnd) : Child() {
        devUpdate = trustHnd->status;
        nwkAddr = trustHnd->newNodeId;
        memcpy(ieeeAddr, trustHnd->newNodeEui64, sizeof(EmberEUI64));


        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + to_string());
    }

    virtual ~Child() {
        devUpdate = EmberDeviceUpdate::EMBER_UNDEFINED;
    }

    uint8_t index;                      // The index of the child of interest.
    EZSP_Bool joining;                  // True if the child is joining. False the child is leaving.
    EmberNodeId nwkAddr;                // The node ID of the child.
    EmberEUI64 ieeeAddr;                // The EUI64 of the child.
    EmberNodeType childType;            // The node type of the child.
    EmberDeviceUpdate devUpdate;        // Child device status
    MACCapabilityFlags macCapability;   // MAC Capability flags

    uint8_t addressTableIndex = 0xFF;
    uint8_t sequence = 0;
    bool    send_in_progress;

    const EmberNodeType type() const {
        return childType;
    }

    bool is_in_progress() const {
        return send_in_progress;
    }

    void in_progress(bool progress){
        send_in_progress = progress;
    }

    void set_squence(const uint8_t seq){
        sequence = (seq%256);
    }

    const uint8_t seq() const {
        return sequence;
    }

    const bool added_to_address_table() const {
        return (addressTableIndex != 0xFF);
    }

    void copy_id(EmberNodeId& id){
        id = nwkAddr;
    }

    void copy_Eui64(EmberEUI64& eui64){
        memcpy(eui64, ieeeAddr, sizeof(EmberEUI64));
    }

    void set_ieeeAddr(const ExtendedPanId& addr){
        memcpy(ieeeAddr, addr, sizeof(ExtendedPanId));
    }

    const bool is_has_valid_id() const {
        return (nwkAddr != NoChildId);
    }

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Child: Index:%d Joining:%d ID:%04X Type:%02X DevUpdate:%02X AddrIdx:%02X Seq:%02X",
        index,
        joining,
        nwkAddr,
        childType,
        devUpdate,
        addressTableIndex,
        sequence
        );
        return std::string(buff) + Conv::to_string(ieeeAddr);
    }

    static const EmberNodeId NoChildId = 0xFFFF;
    static const EmberNodeId NoChild = 0L;
};

}//childs
}//zb_ezsp
#endif