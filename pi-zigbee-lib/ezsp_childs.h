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
    Child() {};
    Child(const std::shared_ptr<zb_ezsp::childJoinHandler> childHnd){
        index = childHnd->index;
        joining = childHnd->joining;
        childId = childHnd->childId;
        childType = childHnd->childType;
        devUpdate = EmberDeviceUpdate::EMBER_UNDEFINED;
        memcpy(childEui64, childHnd->childEui64, sizeof(childEui64));

        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " " + to_string());
    }

    virtual ~Child() {
        devUpdate = EmberDeviceUpdate::EMBER_UNDEFINED;
    }

    uint8_t index;              // The index of the child of interest.
    EZSP_Bool joining;          // True if the child is joining. False the child is leaving.
    EmberNodeId childId;        // The node ID of the child.
    EmberEUI64 childEui64;      // The EUI64 of the child.
    EmberNodeType childType;    // The node type of the child.
    EmberDeviceUpdate devUpdate;// Child device status

    uint8_t addressTableIndex = 0xFF;

    const bool added_to_address_table() const {
        return (addressTableIndex != 0xFF);
    }

    void copy_id(EmberNodeId& id){
        id = childId;
    }

    void copy_Eui64(EmberEUI64& eui64){
        memcpy(eui64, childEui64, sizeof(EmberEUI64));
    }

    const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, "Child: Index:%d Joining:%d ID:%04X Type:%02X DevUpdate:%02X AddrIdx:%02X",
        index,
        joining,
        childId,
        childType,
        devUpdate,
        addressTableIndex
        );
        return std::string(buff) + Conv::Eui64_to_string(childEui64);
    }

    static const EmberNodeId NoChild = 0xFFFF;

};

using child_info = std::shared_ptr<Child>;
using child_map = std::map<EmberNodeId, child_info>;

class Childs {
public:
    Childs() : _freeAddressTableIndex(0){

    }

    virtual ~Childs(){

    }

    /**
     * Child operations (Add, Delete, Print, Get)
     */
    void add_child(const std::shared_ptr<childJoinHandler> child) {
        if(_childs.find(child->childId) == _childs.end()){
            _childs[child->childId] = std::make_shared<Child>(child);
        }
    }

    void del_child(const EmberNodeId child_id){
        auto child = _childs.find(child_id);
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
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Current child Id: " + std::to_string(it->second->childId));
                return it->second->childId;
            }
        }
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " No child");
        return Child::NoChild;
    }

    const EmberNodeId get_child() {
        if(_childs.empty())
            return 0;
        auto it = _childs.begin();
        return it->first;
    }

    const child_info get_child_obj(const EmberNodeId childId = 0x0000) {
        if(_childs.empty())
            return child_info();

        if(childId == 0x0000){
            auto it = _childs.begin();
            return it->second;
        }

        auto child = _childs.find(childId);
        if(child != _childs.end()){
            return child->second;
        }

        return child_info();
    }

    void set_child_join_status(const EmberNodeId childId, const bool joining){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Id: " + std::to_string(childId) + " Join:" + std::to_string(joining));
        auto child = get_child_obj(childId);
        if(child){
            child->joining = joining;
        }
    }

    const size_t count_child() {
        return _childs.size();
    }

    void set_active_child(EmberNodeId childId){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " From: " + std::to_string(_child_on_process) + " To:" + std::to_string(childId));
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

