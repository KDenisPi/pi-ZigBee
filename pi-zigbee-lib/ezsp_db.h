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

#include "net.h"

namespace zb_ezsp {

using net_info = std::shared_ptr<net::Network>;
using net_array = std::shared_ptr<std::array<net_info, 20>>;

class  EzspDb {
public:
    EzspDb() {}
    virtual ~EzspDb() {}

    /**
     * Load data from storage
     */
    virtual bool load() = 0;
    virtual bool load_networks(net_array& nets) = 0;
    virtual bool load_childs(std::shared_ptr<childs::Childs>& childs) = 0;

    /**
     * Add data to storage
     */
    virtual bool add_neighbor() {return true;}
    virtual bool add_network() {return true;}
};

}//namespace zb_ezsp

#endif
