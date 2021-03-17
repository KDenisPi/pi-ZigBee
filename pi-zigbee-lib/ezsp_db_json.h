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

#include <unistd.h>
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
    virtual bool load(Config& conf) override {
        logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Loading: " + _config_file);

        bool result = false;
        std::ifstream ijson(_config_file);
        if(!ijson){
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Could not load config file: " + _config_file);
            return result;
        }

        try{
            _conf = json::parse(ijson);
            json::reference config = _conf.at("config");

            conf._ver = config.at("version");

            logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Config version: " + conf.ver());
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
                p_net->panId = get_uint16_from_hex(net, "panId");
                p_net->radioTxPower = get_optional<uint8_t>(net, "radioTxPower", 0);
                p_net->radioChannel = get_mandatory<uint8_t>(net, "radioChannel");
                p_net->joinMethod = static_cast<EmberJoinMethod>(get_optional<uint8_t>(net, "joinMethod", (uint8_t)EmberJoinMethod::EMBER_USE_MAC_ASSOCIATION));

                p_net->nwkManagerId = get_optional<uint16_t>(net, "nwkManagerId", 0);
                p_net->nwkUpdateId = get_optional<uint8_t>(net, "nwkUpdateId", 0);
                p_net->channels = get_optional<uint32_t>(net, "channels", 0);

                json::reference extendedPanId = net.at("extendedPanId");
                byte_array_to_Eui64(extendedPanId, ex_pan);
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
    virtual bool load_childs(ChildsPtr& childs) override{
        return true;
    }

    /**
     * Save data to json format
     */
    virtual bool save(const Config& conf, const net_array& nets, const ChildsPtr& childs) override {

        /**
         * File exist. Rename.
         */
        if(chkfile(_config_file)){
            std::chrono::time_point<std::chrono::system_clock> tp;
            tp = std::chrono::system_clock::now();
            std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
            std::string backup = _config_file + "_" + std::to_string(time_now);

            _config_file = backup;

            //rename filename
            /*
            int res=rename(_config_file.c_str(), backup.c_str());
            if(res != 0){
                logger::log(logger::LLOG::INFO, "json", std::string(__func__) + " Failed to rename " + _config_file + " to: " + backup + " Err: " + std::to_string(res));
                return false;
            }
            */
        }

        //clear JSON object
        _conf.clear();

        json cfg = conf2json(conf);
        _conf["config"] = cfg;

        json networks = networks2json(nets);
        _conf["networks"] = networks;


        std::ofstream ostrm(_config_file, std::ios::binary | std::ios::out);
        if(!ostrm){
            logger::log(logger::LLOG::ERROR, "json", std::string(__func__) + " Could not create config file: " + _config_file);
            return false;
        }

        ostrm << _conf;

        return true;
    }

    /**
     * Export networks information
     */
    const std::string export_networks(net_array& networks){

        return std::string();
    }

private:
    /*
    * Check if file exist and available
    */
    bool chkfile(const std::string& fname){
        int res = access(fname.c_str(), F_OK);
        return (res == 0);
    }

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

    uint16_t get_uint16_from_hex(json::const_reference obj, const std::string& item){
        std::string str = get_mandatory<std::string>(obj, item);
        return std::stoi(str, nullptr, 16);
    }

    /**
     * Serialize config as JSON object
     */
    const json conf2json(const Config& conf) {
        json cfg = json::object();

        json version = conf.ver();
        cfg["version"] = version;

        return cfg;
    }

    /**
     * Serialize Networks as JSON object
     */
    const json networks2json(const net_array& networks) {
        json nets = json::array();

        if(networks){
            for(const auto& netp : *(networks)){
                if(netp){
                    json net = json::object();
                    net["panId"] = uint16_t_to_string(netp->panId);
                    net["extendedPanId"] = Eui64_to_byte_array(netp->extendedPanId);
                    net["radioTxPower"] = netp->radioTxPower;
                    net["radioChannel"] = netp->radioChannel;
                    net["joinMethod"] = netp->joinMethod;
                    net["nwkManagerId"] = netp->nwkManagerId;

                    nets.insert(nets.end(), net);
                }
            }
        }

        return nets;
    }

    void byte_array_to_Eui64(const json::reference extendedPanId, uint8_t ex_pan[8]){
        for(int i = 0; i < extendedPanId.size() && i < 8; i++){
            std::string bt = extendedPanId.at(i);
            ex_pan[i] = std::stoi(bt, nullptr, 16);
        }
    }

    const json Eui64_to_byte_array(const ExtendedPanId eui64){
        json bytes = json::array();
        char buff[6];
        for(int i = 0; i < sizeof(ExtendedPanId); i++){
            std::sprintf(buff, "0x%02X", eui64[i]);
            bytes.insert(bytes.end(), buff);
        }

        return bytes;
    }

    const std::string uint16_t_to_string(const uint16_t val){
        char buff[16];
        std::sprintf(buff, "0x%04X", val);
        return std::string(buff);
    }

    json _conf;
    std::string _config_file;
};



}//namespace zb_ezsp

#endif
