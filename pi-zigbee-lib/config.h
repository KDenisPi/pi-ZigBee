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

#include "logger.h"

namespace zb_ezsp {

enum ConfigType : uint8_t {
    ConfigurationValue =   0,  //Get/Set through GetConfirationValue/SetConfigurationValue
    PolicyValue      ,  //Get?Set through GetPolicy/SetPolicy
};

enum ConfigValueState : uint8_t {
    need_update_value,
    update_in_progress,
    value_cofirmed,
    update_failed   = 0xFF
};

template<typename T>
class ConfigValue {
public:
    ConfigValue(const uint8_t id, const T value, const ConfigType ctype=ConfigType::ConfigurationValue) : _type(ctype), _value(value), _state(ConfigValueState::need_update_value), _id(id) {}
    virtual ~ConfigValue() {}

    const uint8_t state() const {
        return _state;
    }

    const T value() const {
        return _value;
    }

    void set_state(ConfigValueState state){
        _state = state;
    }

    const ConfigType type() const {
        return _type;
    }

    const uint8_t id() const {
        return _id;
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
    /**
     * Add value for checking here
     *
     * Note!! Some values (at least EZSP_CONFIG_STACK_PROFILE, EZSP_CONFIG_SUPPORTED_NETWORKS) should be updated on any case even if Ezsp has correct current value
     */
    Config() {

        /**
            EzspConfigId::EZSP_CONFIG_STACK_PROFILE, 2
            EzspConfigId::EZSP_CONFIG_SECURITY_LEVEL, 5
            EzspConfigId::EZSP_CONFIG_SUPPORTED_NETWORKS, 1
            EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT, 64
        */
        add(std::make_shared<ConfigValue<uint16_t>>(EzspConfigId::EZSP_CONFIG_STACK_PROFILE, 2));
        add(std::make_shared<ConfigValue<uint16_t>>(EzspConfigId::EZSP_CONFIG_SECURITY_LEVEL, 5));
        add(std::make_shared<ConfigValue<uint16_t>>(EzspConfigId::EZSP_CONFIG_SUPPORTED_NETWORKS, 1));
        add(std::make_shared<ConfigValue<uint16_t>>(EzspConfigId::EZSP_CONFIG_PACKET_BUFFER_COUNT, 64));

        /**
            EzspPolicyId::EZSP_TC_KEY_REQUEST_POLICY, TRUE
            EzspPolicyId::EZSP_APP_KEY_REQUEST_POLICY, TRUE
         */
        add(std::make_shared<ConfigValue<uint8_t>>(EzspPolicyId::EZSP_TC_KEY_REQUEST_POLICY, EZSP_ALLOW_TC_KEY_REQUESTS, ConfigType::PolicyValue));
        add(std::make_shared<ConfigValue<uint8_t>>(EzspPolicyId::EZSP_APP_KEY_REQUEST_POLICY, EZSP_ALLOW_APP_KEY_REQUESTS, ConfigType::PolicyValue));
    }

    ~Config() {}

    const std::string ver() const {
        return _ver;
    }

    std::string _ver;

    /**
     *
     */
    void add(const ConfigV cfg){
        logger::log(logger::LLOG::DEBUG, "config", std::string(__func__) + " " + cfg->to_string());
        _configs.push_back(cfg);
    }

    void add(const PolicyV policy){
        logger::log(logger::LLOG::DEBUG, "config", std::string(__func__) + " " + policy->to_string());
        _policy.push_back(policy);
    }

    const ConfigV config_next(const ConfigValueState state) const {
        for(auto cfg : _configs){
            if(state == cfg->state())
                return cfg;
        }

        return ConfigV();
    }

    /**
     *
     */
    const PolicyV policy_next(const ConfigValueState state) const {
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