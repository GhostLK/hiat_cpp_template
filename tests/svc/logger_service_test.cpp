#include "test_data_path.h"
#include "gtest/gtest.h"
#include "logger_service.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>

class LoggerServiceTest : public testing::Test
{
protected:
    void SetUp() override {
        std::string log_file_path = "test_output.log";
        SX_LOG_INIT(sx::level::info, log_file_path);
        spdlog::flush_on(spdlog::level::err);
    }

    void TearDown() override {
        // 清理测试文件
        std::remove("test_output.log");
    }
};

TEST_F(LoggerServiceTest, LoggerServiceFile) 
{
    std::string test_log_file = "test_output.log";
    
    SX_LOG_INFO("Hello, World!");
    SX_LOG_ERROR("Hello, World!2");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    std::ifstream file(test_log_file);
    if (!file.is_open()) {
        FAIL() << "Cannot open log file: " << test_log_file;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    EXPECT_FALSE(content.empty()) << "Log file is empty";
    EXPECT_TRUE(content.find("Hello, World!") != std::string::npos) << "Cannot find 'Hello, World!' in log content";
    EXPECT_TRUE(content.find("Hello, World!2") != std::string::npos) << "Cannot find 'Hello, World!2' in log content";
}

TEST_F(LoggerServiceTest, LoggerServiceParam)
{
    SX_LOG_TRACE("Hello, World! {}", "start");
    SX_LOG_DEBUG("Hello, World! {}", "debug");
    SX_LOG_INFO("Hello, World! {}", "info");
    SX_LOG_WARN("Hello, World! {}", "warn");
    SX_LOG_ERROR("Hello, World! {}", "error");
    SX_LOG_CRITICAL("Hello, World! {}", "critical");

    SUCCEED();
}
