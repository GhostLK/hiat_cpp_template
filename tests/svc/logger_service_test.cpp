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
        std::remove("test_output.log");
    }
};

TEST_F(LoggerServiceTest, LoggerServiceFile) 
{
    std::string test_log_file = "test_output.log";
    
    SX_LOG_TRACE("Hello, World!0");
    SX_LOG_INFO("Hello, World!1");
    SX_LOG_ERROR("Hello, World!2");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    std::ifstream file(test_log_file);
    if (!file.is_open()) {
        FAIL() << "Cannot open log file: " << test_log_file;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_FALSE(content.empty()) << "Log file is empty";
    EXPECT_FALSE(content.find("Hello, World!0") != std::string::npos) << "Should not find 'Hello, World!0 in log content";
    EXPECT_TRUE(content.find("Hello, World!1") != std::string::npos) << "Cannot find 'Hello, World!1' in log content";
    EXPECT_TRUE(content.find("Hello, World!2") != std::string::npos) << "Cannot find 'Hello, World!2' in log content";
}
