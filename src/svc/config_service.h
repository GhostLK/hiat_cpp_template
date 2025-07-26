#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace sx
{

template <typename Derived>
class ConfigServiceBase
{
public:
    bool load_config(const std::string& config_file_path) {
        return static_cast<Derived*>(this)->load_config(config_file_path);
    }

    template <typename ConfigType>
    ConfigType get_system_config() {
        return static_cast<Derived*>(this)->get_system_config();
    }

    template <typename ConfigType>
    ConfigType get_module_config(const std::string& module_name) {
        return static_cast<Derived*>(this)->get_module_config(module_name);
    }
};

class JsonConfigService : public ConfigServiceBase<JsonConfigService>
{
public:
    static JsonConfigService& get_instance() {
        static JsonConfigService instance;
        return instance;
    }

    bool load_config(const std::string& config_file_path) {
        config_file_path_ = config_file_path;
        std::ifstream ifs(config_file_path);
        if (!ifs.is_open()) {
            return false;
        }
        config_ = json::parse(ifs);
        return true;
    }

    template <typename ConfigType>
    bool get_system_config(ConfigType& out) {
        if (config_.contains("system")) {
            out = config_["system"].get<ConfigType>();
            return true;
        }
        out = ConfigType();
        return false;
    }

    template <typename ConfigType>
    bool get_module_config(const std::string& module_name, ConfigType& out) {
        if (config_.contains(module_name)) {
            out = config_[module_name].get<ConfigType>();
            return true;
        }
        out = ConfigType();
        return false;
    }

    JsonConfigService(const JsonConfigService&) = delete;
    JsonConfigService& operator=(const JsonConfigService&) = delete;
    JsonConfigService(JsonConfigService&&) = delete;
    JsonConfigService& operator=(JsonConfigService&&) = delete;

private:
    using json = nlohmann::json;
    JsonConfigService() = default;
    ~JsonConfigService() = default;

    std::string config_file_path_;
    json config_;
};

using ConfigService = JsonConfigService;

}  // namespace sx


#define SX_LOAD_CONFIG(config_file_path) \
    ::sx::ConfigService::get_instance().load_config(config_file_path)

#define SX_GET_SYSTEM_CONFIG(cfg) \
    ::sx::ConfigService::get_instance().get_system_config(cfg)

#define SX_GET_MODULE_CONFIG(cfg, module_name) \
    ::sx::ConfigService::get_instance().get_module_config(module_name, cfg)

