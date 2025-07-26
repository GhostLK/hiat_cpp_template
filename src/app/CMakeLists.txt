# Create executable
set(CONFIG_PATH ${PROJECT_SOURCE_DIR}/config/config.json)
add_executable(seeker
    main.cpp
)
target_compile_features(seeker PRIVATE cxx_std_17)
target_include_directories(seeker
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}    
)

configure_file(config_path.h.in ${CMAKE_CURRENT_BINARY_DIR}/config_path.h)
