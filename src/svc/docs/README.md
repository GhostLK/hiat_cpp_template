# SVC 服务库使用指南

## 概述

SVC (Service) 是一个研发团队维护的轻量级的 C++ 服务库，提供了常用的系统服务功能，包括配置管理、日志记录、错误处理、性能分析和系统资源管理。所有服务都采用单例模式设计，并提供统一的宏接口。

## 服务列表

- **ConfigService** - JSON 配置文件管理
- **LoggerService** - 基于 spdlog 的日志系统
- **ErrorService** - 分层错误码管理
- **ProfilerService** - Tracy 性能分析集成
- **SystemResourceService** - CPU 核心绑定和线程优先级管理

---

## 0. 初始化
所有函数在main函数进行统一初始化，初始化过程将会读取配置文件，并设置日志等级

```cpp
SystemConfig sys_config;
SX_INIT_SERVICES(CONFIG_PATH, sys_config);
```

## 1. 配置服务 (ConfigService)

### 功能描述
提供 JSON 配置文件的加载和解析功能，支持系统配置和模块配置的分层管理。

### 使用方法

```cpp
#include "config_service.h"

// 定义配置结构体
struct SystemConfig {
    int log_level;
    std::string log_file;
    int max_threads;

    
};

// 定义模块配置
struct NetworkConfig {
    std::string host;
    int port;
    int timeout;
};

// 使用示例
int func() {
    // 1. 获取系统配置
    SystemConfig sys_config;
    if (SX_GET_SYSTEM_CONFIG(sys_config)) {
        std::cout << "Log level: " << sys_config.log_level << std::endl;
    }
    
    // 3. 获取模块配置
    NetworkConfig net_config;
    if (SX_GET_MODULE_CONFIG(net_config, "network")) {
        std::cout << "Server: " << net_config.host << ":" << net_config.port << std::endl;
    }
    
    return 0;
}
```

### 配置文件格式 (config.json)
```json
{
    "system": {
        "log_level": 2,
        "log_file": "logs/app.log",
        "max_threads": 4
    },
    "network": {
        "host": "localhost",
        "port": 8080,
        "timeout": 30
    }
}
```

### JSON 转换支持

配置结构体需要支持 nlohmann::json 的转换，有以下几种方法：

#### 方法1：非侵入式宏（推荐）
```cpp
struct SystemConfig {
    int log_level;
    std::string log_file;
    int max_threads;
};

// 在结构体外部定义，保持代码简洁
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SystemConfig, log_level, log_file, max_threads);
```

#### 方法2：侵入式宏
```cpp
struct SystemConfig {
    int log_level;
    std::string log_file;
    int max_threads;
    
    // 在结构体内部定义
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SystemConfig, log_level, log_file, max_threads);
};
```

#### 方法3：手动实现转换函数
```cpp
struct SystemConfig {
    int log_level;
    std::string log_file;
    int max_threads;
};

void to_json(nlohmann::json& j, const SystemConfig& config) {
    j = nlohmann::json{
        {"log_level", config.log_level}, 
        {"log_file", config.log_file}, 
        {"max_threads", config.max_threads}
    };
}

void from_json(const nlohmann::json& j, SystemConfig& config) {
    j.at("log_level").get_to(config.log_level);
    j.at("log_file").get_to(config.log_file);
    j.at("max_threads").get_to(config.max_threads);
}
```

### 注意事项
- **推荐使用非侵入式宏**：保持结构体定义简洁，转换逻辑与数据分离
- 如果配置项不存在，函数返回 false 并设置默认值
- 配置文件路径相对于程序执行目录
- JSON 字段名必须与结构体成员名一致（或手动映射）

---

## 2. 日志服务 (LoggerService)

### 功能描述
基于 spdlog 的高性能日志系统，支持控制台和文件输出，提供多种日志级别。

### 使用方法

```cpp
#include "logger_service.h"

int func() {
    // 1. 使用各种日志级别
    SX_LOG_TRACE("This is a trace message");
    SX_LOG_DEBUG("Debug info: value = {}", 42);
    SX_LOG_INFO("Application started successfully");
    SX_LOG_WARN("This is a warning: {}", "memory usage high");
    SX_LOG_ERROR("Error occurred: code = {}", error_code);
    SX_LOG_CRITICAL("Critical error: system shutdown");
    
    return 0;
}
```

### 日志级别
```cpp
namespace sx::level {
    enum level_enum {
        trace = 0,    // 最详细的调试信息
        debug = 1,    // 调试信息
        info = 2,     // 一般信息
        warn = 3,     // 警告信息
        err = 4,      // 错误信息
        critical = 5, // 严重错误
        off = 6       // 关闭日志
    };
}
```

### 注意事项
- 日志文件会自动创建目录
- 错误级别及以上的日志会自动刷新到磁盘
- 支持 fmt 格式化语法
- 如果不指定文件名，只输出到控制台

---

## 3. 错误服务 (ErrorService)

### 功能描述
提供分层的错误码管理系统，支持模块化错误定义和自动错误描述。

### 使用方法

注意，需统一在 `common/error_code.h`, `common/error_code.cpp`  下进行注册

```cpp
#include "error_service.h"

// 1. 定义模块错误码枚举（推荐做法）
namespace NetworkError {
    enum Code {
        CONNECTION_TIMEOUT = 1,
        INVALID_HOST_ADDRESS = 2,
        DNS_RESOLUTION_FAILED = 3,
        PORT_UNREACHABLE = 4
    };
}

namespace DatabaseError {
    enum Code {
        CONNECTION_FAILED = 1,
        QUERY_SYNTAX_ERROR = 2,
        TABLE_NOT_FOUND = 3,
        PERMISSION_DENIED = 4
    };
}

// 2. 注册错误模块（在全局范围）
SX_REGISTER_ERROR_MODULE(network, 0x1000);
SX_REGISTER_ERROR_MODULE(database, 0x2000);

// 3. 添加错误定义（在初始化函数中）
void init_errors() {
    SX_ADD_MODULE_ERROR(network, NetworkError::CONNECTION_TIMEOUT, "Connection timeout");
    SX_ADD_MODULE_ERROR(network, NetworkError::INVALID_HOST_ADDRESS, "Invalid host address");
    SX_ADD_MODULE_ERROR(network, NetworkError::DNS_RESOLUTION_FAILED, "DNS resolution failed");
    SX_ADD_MODULE_ERROR(network, NetworkError::PORT_UNREACHABLE, "Port unreachable");
    
    SX_ADD_MODULE_ERROR(database, DatabaseError::CONNECTION_FAILED, "Database connection failed");
    SX_ADD_MODULE_ERROR(database, DatabaseError::QUERY_SYNTAX_ERROR, "SQL query syntax error");
    SX_ADD_MODULE_ERROR(database, DatabaseError::TABLE_NOT_FOUND, "Table not found");
    SX_ADD_MODULE_ERROR(database, DatabaseError::PERMISSION_DENIED, "Database permission denied");
}

// 4. 使用错误码
SX_ErrorCode connect_to_server() {
    if (connection_timeout) {
        return SX_MAKE_ERROR_CODE(network, NetworkError::CONNECTION_TIMEOUT);
    }
    if (invalid_host) {
        return SX_MAKE_ERROR_CODE(network, NetworkError::INVALID_HOST_ADDRESS);
    }
    return sx::SUCCESS;
}

SX_ErrorCode execute_query(const std::string& sql) {
    if (syntax_error) {
        return SX_MAKE_ERROR_CODE(database, DatabaseError::QUERY_SYNTAX_ERROR);
    }
    if (table_missing) {
        return SX_MAKE_ERROR_CODE(database, DatabaseError::TABLE_NOT_FOUND);
    }
    return sx::SUCCESS;
}

// 5. 错误处理
int main() {
    init_errors();
    
    auto result = connect_to_server();
    if (result != sx::SUCCESS) {
        std::string error_msg = SX_GET_ERROR_DESCRIPTION(result);
        SX_LOG_ERROR("Network operation failed: {}", error_msg);
        return -1;
    }
    
    auto query_result = execute_query("SELECT * FROM users");
    if (query_result != sx::SUCCESS) {
        std::string error_msg = SX_GET_ERROR_DESCRIPTION(query_result);
        SX_LOG_ERROR("Database operation failed: {}", error_msg);
        return -1;
    }
    
    return 0;
}
```

### 错误码结构
```
32位错误码 = [20位模块基码] + [12位本地错误码]
示例: 0x1001 = 模块基码 0x1000 + 本地错误码 0x001
```

### 注意事项
- 模块基码必须是 4KB 对齐的（低12位为0）
- 本地错误码范围：0-4095 (0xFFF)
- 错误码 0 表示成功
- 模块注册应在程序启动时完成

### 最佳实践
- **使用枚举定义错误码**：为每个模块定义命名空间和枚举，提高可读性
- **错误码从1开始**：避免使用0作为错误码（0表示成功）
- **分组命名**：将相关的错误码分组，如网络相关、数据库相关等
- **描述性名称**：错误码名称应该清楚描述错误类型
- **文档化**：为每个错误码提供清晰的描述信息

### 高级用法：组织错误码头文件

对于大型项目，建议创建专门的错误码头文件：

```cpp
// errors.h - 项目错误码定义
#pragma once
#include "error_service.h"

namespace MyProject {
namespace Error {

// 网络模块
namespace Network {
    enum Code {
        CONNECTION_TIMEOUT = 1,
        INVALID_HOST = 2,
        DNS_FAILED = 3,
        SSL_ERROR = 4
    };
}

// 数据库模块  
namespace Database {
    enum Code {
        CONNECTION_FAILED = 1,
        QUERY_ERROR = 2,
        TRANSACTION_FAILED = 3,
        SCHEMA_ERROR = 4
    };
}

} // namespace Error
} // namespace MyProject

// 模块注册
SX_REGISTER_ERROR_MODULE(network, 0x1000);
SX_REGISTER_ERROR_MODULE(database, 0x2000);

// 便利宏，简化使用
#define NET_ERROR(code) SX_MAKE_ERROR_CODE(network, MyProject::Error::Network::code)
#define DB_ERROR(code) SX_MAKE_ERROR_CODE(database, MyProject::Error::Database::code)

// 初始化函数
inline void init_all_errors() {
    // 网络错误
    SX_ADD_MODULE_ERROR(network, MyProject::Error::Network::CONNECTION_TIMEOUT, "Network connection timeout");
    SX_ADD_MODULE_ERROR(network, MyProject::Error::Network::INVALID_HOST, "Invalid host address");
    
    // 数据库错误
    SX_ADD_MODULE_ERROR(database, MyProject::Error::Database::CONNECTION_FAILED, "Database connection failed");
    SX_ADD_MODULE_ERROR(database, MyProject::Error::Database::QUERY_ERROR, "SQL query error");
}
```

使用示例：
```cpp
#include "errors.h"

sx::ErrorCode connect_database() {
    if (connection_failed) {
        return DB_ERROR(CONNECTION_FAILED);  // 清晰易读
    }
    return sx::SUCCESS;
}
```

---

## 4. 性能分析服务 (ProfilerService)

### 功能描述
集成 Tracy 性能分析器，提供函数耗时、线程活动和自定义指标的可视化分析。

### 使用方法

```cpp
#include "profiler_service.h"

void expensive_function() {
    SX_PROFILER_FUNC();  // 自动分析整个函数
    
    // 手动分析代码段
    {
        SX_PROFILER_ZONE("Database Query");
        execute_sql_query();
    }
    
    {
        SX_PROFILER_ZONE_COLOR("Network I/O", 0xFF0000);  // 红色标记
        send_network_request();
    }
    
    // 记录自定义指标
    SX_PROFILER_PLOT("Memory Usage MB", get_memory_usage());
    SX_PROFILER_PLOT("Active Connections", connection_count);
}

void game_loop() {
    while (running) {
        SX_PROFILER_ZONE("Frame");
        
        update_game_logic();
        render_frame();
        
        SX_PROFILER_FRAME_MARK();  // 标记帧结束
        SX_PROFILER_MSG("Frame completed");
    }
}
```

### Tracy 服务器连接
1. 启动你的应用程序（客户端会监听 8086 端口）
2. 启动 Tracy 服务器：`tracy-profiler`
3. 在 Tracy 中连接到 `localhost`

### 注意事项
- 只有在 `SX_PROFILER_ENABLE` 宏定义时才启用性能分析
- Tracy 使用反向连接：客户端监听，服务器连接
- 性能分析会有轻微的运行时开销
- 确保 Tracy 服务器版本与客户端库版本匹配

---

## 5. 系统资源服务 (SystemResourceService)

### 功能描述
提供 CPU 核心绑定、线程优先级设置等系统资源管理功能（目前仅支持 Linux）。

### 使用方法

```cpp
#include "system_resource_service.h"
#include <thread>

void worker_thread(int cpu_id) {
    // 绑定当前线程到指定 CPU 核心
    if (SX_BIND_TO_CPU(cpu_id)) {
        SX_LOG_INFO("Thread bound to CPU {}", cpu_id);
    }
    
    // 设置线程优先级
    if (SX_SET_THREAD_PRIORITY(10)) {
        SX_LOG_INFO("Thread priority set to 10");
    }
    
    // 执行工作
    while (running) {
        do_work();
    }
}

int main() {
    // 预分配 CPU 核心
    if (SX_ALLOCATE_CPU_CORES({0, 1, 2, 3})) {
        SX_LOG_INFO("CPU cores allocated successfully");
    }
    
    // 创建工作线程
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back(worker_thread, i);
    }
    
    // 等待线程结束
    for (auto& worker : workers) {
        worker.join();
    }
    
    return 0;
}
```

### 注意事项
- 仅在 Linux 系统上有效，其他系统使用空实现
- CPU 绑定需要适当的权限
- 线程优先级设置可能需要 root 权限
- 最大支持 32 个 CPU 核心

---

## 6. 服务初始化 (svc.h)

### 功能描述
提供统一的服务初始化入口，简化多个服务的集成。

### 使用方法

```cpp
#include "svc.h"

// 定义系统配置
struct AppConfig {
    int log_level = 2;
    std::string log_file = "logs/app.log";
    int max_threads = 4;
};

int main() {
    AppConfig config;
    
    // 一键初始化所有服务
    SX_INIT_SERVICES("config/app.json", config);
    
    // 现在可以使用所有服务
    SX_LOG_INFO("Application initialized");
    SX_PROFILER_MSG("Services ready");
    
    return 0;
}
```

---

## 完整示例

```cpp
#include "svc.h"
#include <thread>
#include <chrono>

// 定义应用程序错误码
namespace AppError {
    enum Code {
        INITIALIZATION_FAILED = 1,
        WORKER_THREAD_ERROR = 2,
        CONFIG_LOAD_ERROR = 3,
        RESOURCE_ALLOCATION_ERROR = 4
    };
}

// 注册错误模块
SX_REGISTER_ERROR_MODULE(app, 0x1000);

struct AppConfig {
    int log_level = 2;
    std::string log_file = "logs/demo.log";
    int worker_threads = 2;
};

void init_errors() {
    SX_ADD_MODULE_ERROR(app, AppError::INITIALIZATION_FAILED, "Application initialization failed");
    SX_ADD_MODULE_ERROR(app, AppError::WORKER_THREAD_ERROR, "Worker thread encountered an error");
    SX_ADD_MODULE_ERROR(app, AppError::CONFIG_LOAD_ERROR, "Failed to load configuration file");
    SX_ADD_MODULE_ERROR(app, AppError::RESOURCE_ALLOCATION_ERROR, "Failed to allocate system resources");
}

void worker_task(int id) {
    SX_PROFILER_FUNC();
    SX_LOG_INFO("Worker {} started", id);
    
    for (int i = 0; i < 5; ++i) {
        SX_PROFILER_ZONE("Work Iteration");
        SX_LOG_DEBUG("Worker {} iteration {}", id, i);
        
        // 模拟工作负载
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        SX_PROFILER_PLOT("Worker Load", id * 10 + i);
    }
    
    SX_LOG_INFO("Worker {} finished", id);
}

int main() {
    // 初始化服务
    AppConfig config;
    SX_INIT_SERVICES("config/demo.json", config);
    
    // 初始化错误码
    init_errors();
    
    SX_LOG_INFO("=== Demo Application Started ===");
    SX_PROFILER_MSG("Application startup complete");
    
    // 创建工作线程
    std::vector<std::thread> workers;
    for (int i = 0; i < config.worker_threads; ++i) {
        workers.emplace_back(worker_task, i);
    }
    
    // 等待完成
    for (auto& worker : workers) {
        worker.join();
    }
    
    SX_LOG_INFO("=== Demo Application Finished ===");
    return 0;
}
```

## 编译要求

### CMake 配置
```cmake
# 启用性能分析
option(SX_ENABLE_PROFILER "Enable Tracy profiler" ON)

if(SX_ENABLE_PROFILER)
    add_definitions(-DSX_PROFILER_ENABLE)
    add_definitions(-DTRACY_ENABLE)
endif()

# 链接 svc 库
target_link_libraries(your_target
    PRIVATE
        svc
)
```

### 依赖项
- **nlohmann/json** - JSON 配置解析
- **spdlog** - 日志系统
- **Tracy** - 性能分析（可选）
- **pthread** - Linux 线程管理

## 最佳实践

1. **统一初始化**：使用 `SX_INIT_SERVICES` 统一初始化服务
2. **错误处理**：为每个模块定义清晰的错误码
3. **性能分析**：在关键路径使用 `SX_PROFILER_ZONE`
4. **日志规范**：使用合适的日志级别，避免在生产环境输出过多调试信息
5. **资源管理**：谨慎使用 CPU 绑定和优先级设置

## 故障排除

### 常见问题

1. **配置文件找不到**
   - 确保配置文件路径相对于程序执行目录
   - 检查文件读取权限

2. **Tracy 连接失败**
   - 确认防火墙设置允许 8086 端口
   - 检查 Tracy 服务器和客户端版本是否匹配

3. **日志文件创建失败**
   - 确保日志目录存在或程序有创建权限
   - 检查磁盘空间

4. **CPU 绑定失败**
   - 检查是否有足够的系统权限
   - 确认 CPU 核心 ID 有效

---

*如有问题或建议，请联系开发团队。*
