/**
 * ezsp_db.h
 *
 *  EZSP database
 *
 *  Created on: Jul 1, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_DB_H_
#define PI_ZIGBEE_LIB_EZSP_DB_H_

#include <map>
#include <array>

namespace zb_ezsp {

using net_info = std::shared_ptr<EmberNetworkParameters>;
using net_array = std::array<net_info, 20>;

using child_info = std::shared_ptr<childJoinHandler>;
using child_map = std::map<EmberNodeId, child_info>;

class  EzspDb {
public:
    EzspDb() {}
    virtual ~EzspDb() {}

    /**
     * Load data from storage
     */
    virtual bool load() = 0;
    virtual bool load_networks(net_array& nets) = 0;
    virtual bool load_childs(child_map& nets) = 0;

    /**
     * Add data to storage
     */
    virtual bool add_neighbor() {return true;}
    virtual bool add_network() {return true;}
};

}//namespace zb_ezsp

#endif
