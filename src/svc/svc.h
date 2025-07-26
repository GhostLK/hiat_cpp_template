#pragma once

#include "logger_service.h"
#include "config_service.h"
#include "profiler_service.h"
#include "error_service.h"
#include "system_resource_service.h"
#include <string>

namespace sx
{

template <typename SysConfig>
void init_svc(const std::string& config_file_path, SysConfig& sys_config)
{
    SX_LOAD_CONFIG(config_file_path);
    SX_GET_SYSTEM_CONFIG(sys_config);
    SX_LOG_INIT(level::from_string(sys_config.log_level), sys_config.log_file);
    SX_LOG_INFO("Initializing services...");
    SX_LOG_INFO("Log level: {}", sys_config.log_level);
    SX_LOG_INFO("Log file: {}", sys_config.log_file);
}

}  // namespace sx

#define SX_INIT_SERVICES(config_file_path, sys_config) ::sx::init_svc(config_file_path, sys_config)
