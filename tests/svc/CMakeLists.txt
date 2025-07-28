set(TEST_DATA_DIR ${CMAKE_CURRENT_LIST_DIR}/../data/svc)
set(TEST_JSON_PATH ${TEST_DATA_DIR}/system_config_test.json)
set(TEST_LOGGER_JSON_PATH ${TEST_DATA_DIR}/logger_config_test.json)
configure_file(${CMAKE_CURRENT_LIST_DIR}/test_data_path.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_data_path.h)

add_executable(svc_test 
    config_service_test.cpp
    logger_service_test.cpp
    profiler_service_test.cpp
)
target_link_libraries(svc_test 
    PRIVATE 
        svc
)
target_include_directories(svc_test 
    PRIVATE 
        ${CMAKE_CURRENT_BINARY_DIR}
)
AddTests(svc_test)
