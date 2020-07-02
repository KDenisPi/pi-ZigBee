/**
 * ezsp_db_json.h
 *
 *  EZSP JSON based database
 *
 *  Created on: Jul 1, 2020
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_EZSP_DB_JSON_H_
#define PI_ZIGBEE_LIB_EZSP_DB_JSON_H_

#include <fstream>
#include <nlohmann/json.hpp>

#include "logger.h"
#include "ezsp_db.h"

namespace zb_ezsp {

using json = nlohmann::json;

class  EzspDbJson {
public:
    EzspDbJson() {}
    virtual ~EzspDbJson() {}

    /**
     * Load data from storage
     */
    virtual bool load(const std::string& config_file){
        bool result = false;

        logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Loading: " + config_file);

        _config_file = config_file;
        std::ifstream ijson(_config_file);
        if(!ijson){
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Could not load config file: " + config_file);
            return result;
        }

        try{
            _conf = json::parse(ijson);

            std::string cfg_ver = _conf.at("version");

            logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Config version: " + cfg_ver);
            result = true;
        }
        catch (json::parse_error& e){
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Could not parse config file. Error: " + std::to_string(e.id) + " Description: " + std::string(e.what()));
        }
        catch (json::out_of_range& e)
        {
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Invalid data. Error: " + std::to_string(e.id) + " Description: " + std::string(e.what()));
        }

        ijson.close();

        return result;
    }

    /**
     *
     */
    virtual bool load_networks(net_array& networks){
        bool result = true;
        try
        {
            json::reference netws = _conf.at("networks");
            int net_idx = 0;
            for(auto& net : netws.items()){
                std::shared_ptr<EmberNetworkParameters> p_net = std::make_shared<EmberNetworkParameters>();

                p_net->panId = get_mandatory<uint16_t>(net, "panId");
                p_net->radioTxPower = get_optional<uint8_t>(net, "radioTxPower", 0);
                p_net->radioChannel = get_mandatory<uint8_t>(net, "radioChannel");
                p_net->joinMethod = static_cast<EmberJoinMethod>(get_optional<uint8_t>(net, "joinMethod", (uint8_t)EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION));
                p_net->nwkManagerId = get_mandatory<uint16_t>(net, "nwkManagerId");
                p_net->nwkUpdateId = get_optional<uint8_t>(net, "nwkUpdateId", 0);
                p_net->channels = get_optional<uint32_t>(net, "channels", 0);

                uint8_t ex_pan[8];
                json::reference extendedPanId = netws.at("extendedPanId");
                for(int i = 0; i < extendedPanId.size() && i < 8; i++){
                    ex_pan[i] = extendedPanId.at(i);
                }

                networks[net_idx++] = p_net;
            }
        }
        catch (json::out_of_range& e)
        {
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Out of range Error: " + std::to_string(e.id) + " Description: " + std::string(e.what()));
            result = false;
        }

        return result;
    }

    /**
     *
     */
    virtual bool load_childs(child_map& nets){
        return true;
    }

    /**
     * Add data to storage
     */
    virtual bool add_neighbor() {return true;}
    virtual bool add_network() {return true;}

private:
    template<class T>
    T get_mandatory(json::const_reference obj, const std::string& key){
        return obj.at(key);
    }

    template<class T>
    T get_optional(json::const_reference obj, const std::string& key, const T defval){
        T result;
        try{
            result =  obj.at(key);
        }
        catch (json::out_of_range& e)
        {
            return defval;
        }

        return result;
    }


public:
    json _conf;
    std::string _config_file;

};

}//namespace zb_ezsp

#endif
