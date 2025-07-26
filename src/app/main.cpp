#include <iostream>
#include "config_path.h"
#include "svc.h"


struct SystemConfig {
    std::string log_file;
    std::string log_level;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SystemConfig, log_file, log_level);
};


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    SystemConfig sys_config;
    SX_INIT_SERVICES(CONFIG_PATH, sys_config);
    SX_LOG_INFO("===========================================");
    SX_LOG_INFO("============== Start running ==============");
    SX_LOG_INFO("===========================================");

    // Run some functions
    // ...

    return 0;
}
