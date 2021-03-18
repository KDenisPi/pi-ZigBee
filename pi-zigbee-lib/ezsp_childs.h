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
#include "child.h"
namespace zb_ezsp {
namespace childs {

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

    void add(const u_int64_t id, const child_info child){
        _childs[id] = child;
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

    const child_map& get_childs() const {
        return _childs;
    }

private:
    child_map _childs;

    EmberNodeId _child_on_process;  //Node Id for child we have not finished actions
    uint8_t _freeAddressTableIndex; //Address Table Index can be used for a new record (use stack here)

};

}//childs

using ChildsPtr = std::shared_ptr<childs::Childs>;

}//zb_ezsp

#endif

