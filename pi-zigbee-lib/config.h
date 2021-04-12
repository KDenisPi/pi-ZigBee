/**
 * config.h
 *
 *  Configuration parameters
 *
 *  Created on: Mar 15, 2021
 *      Author: Denis Kudia
 */
#ifndef PI_ZIGBEE_LIB_CONFIG_H_
#define PI_ZIGBEE_LIB_CONFIF_H_

#include <string>
#include <list>
#include <memory>

namespace zb_ezsp {

enum ConfigType : uint8_t {
    ConfigurationValue =   0,  //Get/Set through GetConfirationValue/SetConfigurationValue
    PolicyValue      ,  //Get?Set through GetPolicy/SetPolicy
};

enum ConfigValueState : uint8_t {
    not_verufied    = 0,
    veryfy_in_progress,
    need_update,
    update_in_progress,
    update_failed   = 0xFF
};

template<typename T>
class ConfigValue {
public:
    ConfigValue(const uint8_t id, const T value, const ConfigType ctype=ConfigType::ConfigurationValue) : _type(ctype), _value(value), _state(ConfigValueState::not_verufied), _id(id) {}
    virtual ~ConfigValue() {}

    const uint8_t state() const {
        return _state;
    }

    const std::string to_string() const{
        return std::string((_type==ConfigType::ConfigurationValue) ? " Cfg " : " Policy ") + " Id: "+ std::to_string(_id) + " Val: " + std::to_string(_value) + " State: " + std::to_string(_state);
    }

    ConfigValueState _state;     // 0 - not verified, 1 - verify is in progress, 2 - need update value, 3 - value updated, 0xFF - update/get value error
    ConfigType _type;
    uint8_t _id;
    T _value;
};

using ConfigV = std::shared_ptr<ConfigValue<uint16_t>>;
using PolicyV = std::shared_ptr<ConfigValue<uint8_t>>;
class Config {
public:
    Config() {}
    ~Config() {}

    const std::string ver() const {
        return _ver;
    }

    std::string _ver;

    /**
     *
     */
    const ConfigV config_next(const ConfigValueState state = ConfigValueState::not_verufied) const {
        for(auto cfg : _configs){
            if(state == cfg->state())
                return cfg;
        }

        return ConfigV();
    }

    /**
     *
     */
    const PolicyV policy_next(const ConfigValueState state = ConfigValueState::not_verufied) const {
        for(auto policy : _policy){
            if(state == policy->state())
                return policy;
        }

        return PolicyV();
    }

    const std::string to_string() const {
        std::string result;

        result = " Ver: " + _ver;
        result += " Config ";
        for(auto cfg : _configs){
            result += cfg->to_string() + "\n";
        }
        result += " Policy ";
        for(auto policy : _policy){
            result += policy->to_string() + "\n";
        }

    }

    std::list<ConfigV> _configs;
    std::list<PolicyV> _policy;
};

} //zb_ezsp

#endif