#include <gtest/gtest.h>
#include "config_service.h"
#include "test_data_path.h"

struct SystemConfig {
    std::string name;
    int idx;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SystemConfig, name, idx);

TEST(ConfigServiceTest, GetSystemConfig) {
    auto& config_service = sx::JsonConfigService::get_instance();
    config_service.load_config(TEST_JSON_PATH);
    SystemConfig config;
    config_service.get_system_config(config);
    EXPECT_EQ(config.name, "test");
    EXPECT_EQ(config.idx, 1);
}

TEST(ConfigServiceTest, GetModuleConfig) {
    auto& config_service = sx::JsonConfigService::get_instance();
    config_service.load_config(TEST_JSON_PATH);
    SystemConfig config;
    config_service.get_module_config("module", config);
    EXPECT_EQ(config.name, "module_test");
    EXPECT_EQ(config.idx, 2);
}

TEST(ConfigServiceTest, GetModuleConfigWithInvalidModuleName) {
    auto& config_service = sx::JsonConfigService::get_instance();
    config_service.load_config(TEST_JSON_PATH);
    SystemConfig config;
    config_service.get_module_config("invalid_module", config);
    EXPECT_EQ(config.name, "");
    EXPECT_EQ(config.idx, 0);
}
    