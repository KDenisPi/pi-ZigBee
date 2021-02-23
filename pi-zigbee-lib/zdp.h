/**
 * zdp.h
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
 *  Created on: Feb 16, 2021
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_ZDP_H_
#define PI_ZIGBEE_LIB_ZDP_H_

#include <cstdint>
#include <stddef.h>
#include <string.h>
#include <algorithm>

#include "logger.h"
#include "ezsp_util.h"
#include "ezsp_types.h"

namespace zb_ezsp {
namespace zdp {

/**
 * 2.4.3 Client Services
 * The Device Profile Client Services support the transport of device and service discovery requests, end de-
 * vice binding requests, bind requests, unbind requests, and network management requests from client to
 * server. Additionally, Client Services support receipt of responses to these requests from the server.
 * 2.4.3.1 Device and Service Discovery Client Services
 */

enum Cluster_ID : uint16_t {
    NWK_addr_req = 0x0000,
    IEEE_addr_req = 0x0001,
    Node_Desc_req = 0x0002,
    Power_Desc_req = 0x0003,
    Simple_Desc_req = 0x0004,
    Active_EP_req = 0x0005,
    Match_Desc_req = 0x0006,
    Complex_Desc_req = 0x0010,
    User_Desc_req = 0x0011,
    Discovery_Cache_req = 0x0012,
    Device_annce = 0x0013,
    Parent_annce = 0x001F,
    User_Desc_set = 0x0014,
    System_Server_Discovery_req = 0x0015,
    Discovery_store_req = 0x0016,
    Node_Desc_store_req = 0x0017,
    Power_Desc_store_req = 0x0018,
    Active_EP_store_req = 0x0019,
    Simple_Desc_store_req = 0x001A,
    Remove_node_cache_req = 0x001B,
    Find_node_cache_req = 0x001C,
    Extended_Simple_Desc_req = 0x001D,
    Extended_Active_EP_req = 0x001E
};

static const bool is_client_zdp(const uint16_t cluster_id){
    return (cluster_id >= Cluster_ID::NWK_addr_req && cluster_id <= Parent_annce);
}

static const bool is_zdp(const uint16_t cluster_id){
    return is_client_zdp(cluster_id);
}

class Device_Annce;

class Zdp{
public:
    Zdp(const Cluster_ID id) : _id(id) {}
    ~Zdp() {}

    static std::shared_ptr<Zdp> create(const uint16_t cluster_id, const uint8_t* buff, const size_t max_len, size_t& pos){
        logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Cluster ID: " + Conv::to_string(cluster_id));

        if( !is_client_zdp(cluster_id)){
            return std::shared_ptr<Zdp>();
        }

        std::shared_ptr<Zdp> zdp_obj;
        switch(cluster_id){
            case Cluster_ID::Device_annce:
            {
                zdp_obj = std::static_pointer_cast<Zdp>(create_zdp<Device_Annce>());

            }
            break;
            default:
            {
                logger::log(logger::LLOG::DEBUG, "ezsp", std::string(__func__) + " Cluster ID: " + Conv::to_string(cluster_id) + " Not recognized");
            }
        }

        if(zdp_obj){
            if(zdp_obj->len() > max_len)
                zdp_obj.reset();
            else
                pos = zdp_obj->load(buff, pos);
        }

        return zdp_obj;
    }

    virtual void process(std::shared_ptr<childs::Childs>& childs) {}

    template<typename T>
    static std::shared_ptr<T> create_zdp(){
        return std::make_shared<T>();
    }

    virtual size_t load(const uint8_t* buff, size_t& pos) = 0;
    virtual size_t len() const  = 0;

    const Cluster_ID get_id() const {
        return _id;
    }

    virtual const std::string to_string() const {
        char buff[128];
        std::sprintf(buff, " ZDP Cluster ID: %04X ", _id);
        return std::string(buff);
    }

protected:
    Cluster_ID _id;
};
/**
 * 2.4.3.1.11.1
 * When Generated
 * The Device_annce is provided to enable ZigBee devices on the network to notify other ZigBee devices that the device has joined or re-joined the network, identifying the device’s 64-bit IEEE address and new 16-bit
 * NWK address, and informing the Remote Devices of the capability of the ZigBee device. This command shall be invoked for all ZigBee end devices upon join or rejoin. This command may also be invoked by
 * ZigBee routers upon join or rejoin as part of NWK address conflict resolution. The destination addressing on this primitive is broadcast to all devices for which macRxOnWhenIdle = TRUE.
 */
class Device_Annce : public Zdp {
public:
    Device_Annce() : Zdp(Cluster_ID::Device_annce) {}
    ~Device_Annce() {}

    virtual size_t load(const uint8_t* buff, size_t& pos) override {
        pos = Conv::get(buff, pos, _NwkAddr);
        pos = Conv::get(buff, pos, _IeeeAddr, sizeof(EmberEUI64), sizeof(EmberEUI64));
        pos = Conv::get(buff, pos, _Capability);
        return pos;
    }

    virtual size_t len() const override {
        return sizeof(EmberNodeId) + sizeof(EmberEUI64) + sizeof(uint8_t);
    }

    virtual void process(std::shared_ptr<childs::Childs>& childs) override;

    virtual const std::string to_string() const override {
        char buff[128];
        std::sprintf(buff, " NwkAddr: %04X Capability: %02X ", _NwkAddr, _Capability);
        return Zdp::to_string() + std::string(buff) + Conv::to_string(_IeeeAddr);
    }

protected:
    EmberNodeId _NwkAddr;    // Device Address 16-bit NWK address NWK address for the Local Device
    EmberEUI64  _IeeeAddr;   // Device Address 64-bit IEEE address IEEE address for the Local Device
    uint8_t _Capability;     // Bitmap See Figure 2.17 Capability of the local device (MACCapabilityFlags)
};



}//zdp
}//zb_ezsp

#endif
