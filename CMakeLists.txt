cmake_minimum_required(VERSION 3.16)

project(template_project VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)
include(SetOutputDirectories)
include(CTest)
include(SetupClangd)
include(CPack)

option(SX_ENABLE_TEST "Enable test" ON)
option(SX_ENABLE_PROFILER "Enable profiler" ON)

if (SX_ENABLE_PROFILER)
    add_definitions(-DSX_PROFILER_ENABLE)
    add_definitions(-DTRACY_ENABLE)
    include(tracy)
endif()

add_subdirectory(src)
add_subdirectory(tests)
