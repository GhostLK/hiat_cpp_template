#include "gtest/gtest.h"
#include "profiler_service.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <limits>

class ProfilerServiceTest : public testing::Test
{
protected:
    void SetUp() override {
        // 获取profiler服务实例
        auto& service = sx::PerfService::get_instance();
        service.message("Test setup started");
    }

    void TearDown() override {
    }

    void simulate_work(int milliseconds) {
        SX_PROFILER_FUNC();
        auto start = std::chrono::high_resolution_clock::now();
        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if (duration.count() >= milliseconds) break;
        }
    }
};

// 测试基本宏功能
TEST_F(ProfilerServiceTest, BasicMacrosTest) 
{
    SX_PROFILER_FUNC();
    SX_PROFILER_MSG("Basic macros test started");
    
    {
        SX_PROFILER_ZONE("test_zone");
        simulate_work(1);  // 1ms的工作
    }
    
    {
        SX_PROFILER_ZONE_COLOR("colored_zone", 0xFF0000);
        simulate_work(1);  // 1ms的工作
    }
    
    SX_PROFILER_PLOT("test_value", 42.0);
    SX_PROFILER_FRAME_MARK();
    
    SUCCEED();  // 如果能执行到这里说明宏工作正常
}

// 测试PerfService的直接方法调用
TEST_F(ProfilerServiceTest, ServiceMethodsTest)
{
    auto& service = sx::PerfService::get_instance();
    
    // 测试message方法
    service.message("Direct service call test");
    service.message(nullptr);  // 测试null指针处理
    
    // 测试plot方法
    service.plot("cpu_usage", 75.5);
    service.plot("memory_usage", 1024.0);
    service.plot("fps", 60.0);
    
    // 测试frame_mark方法
    service.frame_mark();
    
    SUCCEED();
}

// 测试嵌套作用域
TEST_F(ProfilerServiceTest, NestedScopesTest)
{
    SX_PROFILER_FUNC();
    SX_PROFILER_MSG("Testing nested scopes");
    
    {
        SX_PROFILER_ZONE("outer_scope");
        simulate_work(2);
        
        {
            SX_PROFILER_ZONE("inner_scope_1");
            simulate_work(1);
            
            {
                SX_PROFILER_ZONE("deep_scope");
                simulate_work(1);
            }
        }
        
        {
            SX_PROFILER_ZONE_COLOR("inner_scope_2", 0x00FF00);
            simulate_work(1);
        }
    }
    
    SUCCEED();
}

// 测试连续的frame marks
TEST_F(ProfilerServiceTest, FrameMarksTest)
{
    SX_PROFILER_MSG("Testing frame marks");
    
    for (int i = 0; i < 5; ++i) {
        {
            SX_PROFILER_ZONE("frame_work");
            simulate_work(1);
            SX_PROFILER_PLOT("frame_number", static_cast<double>(i));
        }
        SX_PROFILER_FRAME_MARK();
    }
    
    SUCCEED();
}

// 测试plot功能的各种数值
TEST_F(ProfilerServiceTest, PlotVariousValuesTest)
{
    SX_PROFILER_MSG("Testing plot with various values");
    
    // 测试不同类型的数值
    SX_PROFILER_PLOT("zero_value", 0.0);
    SX_PROFILER_PLOT("positive_int", 123.0);
    SX_PROFILER_PLOT("negative_value", -456.789);
    SX_PROFILER_PLOT("large_value", 1e6);
    SX_PROFILER_PLOT("small_value", 1e-6);
    SX_PROFILER_PLOT("nan_value", std::numeric_limits<double>::quiet_NaN());
    SX_PROFILER_PLOT("inf_value", std::numeric_limits<double>::infinity());
    
    SUCCEED();
}

// 测试长时间运行的场景
TEST_F(ProfilerServiceTest, LongRunningTest)
{
    SX_PROFILER_FUNC();
    SX_PROFILER_MSG("Long running test started");
    
    const int iterations = 10;
    for (int i = 0; i < iterations; ++i) {
        {
            SX_PROFILER_ZONE("iteration");
            simulate_work(1);
            
            // 模拟不同的负载
            double load = 50.0 + 30.0 * std::sin(i * 0.5);
            SX_PROFILER_PLOT("simulated_load", load);
        }
        
        if (i % 3 == 0) {
            SX_PROFILER_FRAME_MARK();
        }
    }
    
    SX_PROFILER_MSG("Long running test completed");
    SUCCEED();
}

// 测试字符串边界情况
TEST_F(ProfilerServiceTest, StringBoundaryTest)
{
    auto& service = sx::PerfService::get_instance();
    
    // 测试各种字符串情况
    service.message("");  // 空字符串
    service.message("A");  // 单字符
    service.message("Normal message");  // 正常消息
    
    // 测试较长的消息
    std::string long_msg(1000, 'X');
    service.message(long_msg.c_str());
    
    // 测试带特殊字符的消息
    service.message("Message with 中文 and émojis 🚀");
    
    SX_PROFILER_MSG("");
    SX_PROFILER_MSG("Macro message test");
    
    // 测试zone名称
    {
        SX_PROFILER_ZONE("");  // 空名称
        simulate_work(1);
    }
    {
        SX_PROFILER_ZONE("A");  // 单字符名称
        simulate_work(1);
    }
    {
        SX_PROFILER_ZONE("Normal Zone Name");
        simulate_work(1);
    }
    
    SUCCEED();
}

// 测试多线程场景（如果Tracy支持）
TEST_F(ProfilerServiceTest, MultiThreadTest)
{
    SX_PROFILER_MSG("Multi-thread test started");
    
    auto worker = [](int thread_id) {
        SX_PROFILER_FUNC();
        
        for (int i = 0; i < 3; ++i) {
            {
                SX_PROFILER_ZONE("worker_task");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                SX_PROFILER_PLOT("thread_work", static_cast<double>(thread_id * 10 + i));
            }
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SX_PROFILER_MSG("Multi-thread test completed");
    SUCCEED();
}
