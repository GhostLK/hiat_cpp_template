# 单元配置测试
#  1. 使用本地googletest
#  2. 增加覆盖率测试
#  3. 增加内存泄漏检测

set(GOOGLETEST_DIR ${PROJECT_SOURCE_DIR}/third_party/googletest)

if(NOT EXISTS ${GOOGLETEST_DIR})
    message(FATAL_ERROR "googletest not found at ${GOOGLETEST_DIR}. Please download it manually for offline deployment.")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
option(INSTALL_GMOCK "Install GMock" OFF)
option(INSTALL_GTEST "Install GTest" OFF)

add_subdirectory(${GOOGLETEST_DIR} ${CMAKE_BINARY_DIR}/googletest)

include(GoogleTest)
include(Coverage)
include(Memcheck)

macro(AddTests target)
  message("Adding tests to ${target}")
  target_link_libraries(${target} PRIVATE gtest_main gmock)
  gtest_discover_tests(${target})
  AddCoverage(${target})
  AddMemcheck(${target}) 
endmacro()
