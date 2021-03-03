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

class  EzspDbJson : public EzspDb {
public:
    EzspDbJson(const std::string& config_file) : _config_file(config_file) {}
    EzspDbJson() {}
    virtual ~EzspDbJson() {}

    void set_config(const std::string& config_file){
        _config_file = config_file;
    }

    /**
     * Load data from storage
     */
    virtual bool load() override {
        logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Loading: " + _config_file);

        bool result = false;
        std::ifstream ijson(_config_file);
        if(!ijson){
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Could not load config file: " + _config_file);
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
    virtual bool load_networks(net_array& networks) override{
        bool result = true;
        uint8_t ex_pan[8];
        int net_idx = 0;

        try
        {
            json::reference netws = _conf.at("networks");
            for(auto& item : netws.items()){
                std::shared_ptr<net::Network> p_net = std::make_shared<net::Network>();

                auto& net = item.value();
                p_net->panId = get_mandatory<uint16_t>(net, "panId");
                p_net->radioTxPower = get_optional<uint8_t>(net, "radioTxPower", 0);
                p_net->radioChannel = get_mandatory<uint8_t>(net, "radioChannel");
                p_net->joinMethod = static_cast<EmberJoinMethod>(get_optional<uint8_t>(net, "joinMethod", (uint8_t)EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION));

                p_net->nwkManagerId = get_optional<uint16_t>(net, "nwkManagerId", 0);
                p_net->nwkUpdateId = get_optional<uint8_t>(net, "nwkUpdateId", 0);
                p_net->channels = get_optional<uint32_t>(net, "channels", 0);

                json::reference extendedPanId = net.at("extendedPanId");
                for(int i = 0; i < extendedPanId.size() && i < 8; i++){
                    std::string bt = extendedPanId.at(i);
                    ex_pan[i] = std::stoi(bt, nullptr, 16);
                }
                p_net->set_ext_pan(ex_pan);

                logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Index: " + std::to_string(net_idx) + " " + p_net->to_string());
                networks->at(net_idx++) = p_net;
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
    virtual bool load_childs(std::shared_ptr<childs::Childs>& childs) override{
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
