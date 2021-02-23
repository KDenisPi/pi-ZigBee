/**
 * ezsp_childs.h
 *
 *  EZSP Childs functions.
 *
 *  Created on: November 30, 2020
 *      Author: Denis Kudia
 */

#ifndef PI_ZIGBEE_LIB_EZSP_CHILDS_H_
#define PI_ZIGBEE_LIB_EZSP_CHILDS_H_

#include <map>

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
    EmberNodeId nwkAddr;                     // The node ID of the child.
    EmberEUI64 ieeeAddr;                   // The EUI64 of the child.
    EmberNodeType childType;            // The node type of the child.
    EmberDeviceUpdate devUpdate;        // Child device status
    MACCapabilityFlags macCapability;   // MAC Capability flags

    uint8_t addressTableIndex = 0xFF;
    uint8_t sequence = 0;
    bool    send_in_progress;

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

using child_info = std::shared_ptr<Child>;
using child_map = std::map<uint64_t, child_info>;

class Childs {
public:
    Childs() : _freeAddressTableIndex(0){

    }

    virtual ~Childs(){

    }

    /**
     * Child operations (Add, Delete, Print, Get)
     */
    const child_info add_child(const std::shared_ptr<childJoinHandler>& child) {
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " child: " + Conv::to_string(child->childId) + " EUI64: " + Conv::to_string(child->childEui64) + " Type: " + std::to_string(child->childType));
        const uint64_t eui64 = Conv::eui642u64(child->childEui64);
        if(_childs.find(eui64) == _childs.end()){
            _childs[eui64] = std::make_shared<Child>(child);
        }
        else {
            if(_childs[eui64]->nwkAddr != child->childId){
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Update ID from child: " + Conv::to_string(_childs[eui64]->nwkAddr) + " to: " + Conv::to_string(child->childId));
                _childs[eui64]->nwkAddr = child->childId;
            }
        }

        return _childs[eui64];
    }

    const child_info add_child(const std::shared_ptr<trustCenterJoinHandler>& child) {
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " child: " + Conv::to_string(child->newNodeId) + " EUI64: " + Conv::to_string(child->newNodeEui64));
        const uint64_t eui64 = Conv::eui642u64(child->newNodeEui64);
        if(_childs.find(eui64) == _childs.end()){
            _childs[eui64] = std::make_shared<Child>(child);
        }
        else {
            _childs[eui64]->devUpdate = child->status;
            if(_childs[eui64]->nwkAddr != child->newNodeId){
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Update ID from child: " + Conv::to_string(_childs[eui64]->nwkAddr) + " to: " + Conv::to_string(child->newNodeId));
                _childs[eui64]->nwkAddr = child->newNodeId;
            }
        }

        return _childs[eui64];
    }


    void del_child(const EmberEUI64& childId){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " child: " + Conv::to_string(childId));

        const uint64_t eui64 = Conv::eui642u64(childId);
        auto child = _childs.find(eui64);
        if(child != _childs.end()){
            /**
             * If child has address table
             */

            _childs.erase(child);
        }
    }

    const std::string list_childs() {
        std::string result;
        for (auto it = _childs.begin(); it != _childs.end(); ++it) {
            result += it->second->to_string() + "\n";
        }
        return result;
    }

    const EmberNodeId get_next_for_address_table() const {
        for (auto it = _childs.begin(); it != _childs.end(); ++it) {
            if(!it->second->added_to_address_table()){
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Current child Id: " + Conv::to_string(it->second->nwkAddr));
                return it->second->nwkAddr;
            }
        }
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " No child");
        return Child::NoChildId;
    }

    const child_info get_child_by_child_id(const EmberNodeId childId){
        for (auto it = _childs.begin(); it != _childs.end(); ++it) {
            if(childId == it->second->nwkAddr){
                return it->second;
            }
        }

        return child_info();
    }

    void invalidate(const EmberEUI64& eue64_id, const EmberNodeId id){
        for (auto it = _childs.begin(); it != _childs.end(); ++it) {
            if(id == it->second->nwkAddr){
                const uint64_t eui64 = Conv::eui642u64(eue64_id);
                if(it->first != eui64){
                    it->second->nwkAddr = Child::NoChildId;
                    logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " No NwkAddr: " + Conv::to_string(id) + " EUI64: " + Conv::to_string(it->second->ieeeAddr));
                }
            }
        }
    }

    const child_info get_child_obj(const EmberEUI64& childId) {
        if(_childs.empty())
            return child_info();

        const uint64_t eui64 = Conv::eui642u64(childId);
        auto child = _childs.find(eui64);
        if(child != _childs.end()){
            return child->second;
        }

        return child_info();
    }

    void set_child_join_status(const EmberEUI64& childId, const bool joining){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Id: " + Conv::to_string(childId) + " Join:" + std::to_string(joining));
        auto child = get_child_obj(childId);
        if(child){
            child->joining = joining;
        }
    }

    const size_t count_child() {
        return _childs.size();
    }

    void set_active_child(EmberNodeId childId){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " From: " + Conv::to_string(_child_on_process) + " To:" + Conv::to_string(childId));
        _child_on_process = childId;
    }

private:
    child_map _childs;

    EmberNodeId _child_on_process;  //Node Id for child we have not finished actions
    uint8_t _freeAddressTableIndex; //Address Table Index can be used for a new record (use stack here)

};


}//childs
}//zb_ezsp

#endif

