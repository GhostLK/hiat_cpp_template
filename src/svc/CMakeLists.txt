# include(Doxygen)
include(nlohmann_json)
include(spdlog)

add_library(svc INTERFACE)
target_include_directories(svc 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(svc
    INTERFACE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)
if (SX_ENABLE_PROFILER)
    target_link_libraries(svc
        INTERFACE
            Tracy::TracyClient
    )
endif()
