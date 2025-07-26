# Setup ASIO header-only library from third_party directory

# Assume ASIO is in third_party directory
set(ASIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/asio/asio/include")

# Check if the directory exists
if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio.hpp")
    message(FATAL_ERROR "ASIO not found at ${ASIO_INCLUDE_DIR}. Please ensure ASIO is installed in third_party/asio-1.30.2/include/")
endif()

# Create imported interface library
if(NOT TARGET asio::asio)
    add_library(asio::asio INTERFACE IMPORTED)
    
    # Set include directories
    set_target_properties(asio::asio PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE;ASIO_NO_DEPRECATED"
        INTERFACE_COMPILE_FEATURES "cxx_std_11"
    )
    
    # Platform-specific settings
    if(WIN32)
        set_target_properties(asio::asio PROPERTIES
            INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE;ASIO_NO_DEPRECATED;_WIN32_WINNT=0x0601"
            INTERFACE_LINK_LIBRARIES "ws2_32;mswsock"
        )
    else()
        # Unix-like systems need pthread
        find_package(Threads REQUIRED)
        set_target_properties(asio::asio PROPERTIES
            INTERFACE_LINK_LIBRARIES "Threads::Threads"
        )
    endif()
    
    message(STATUS "Found ASIO: ${ASIO_INCLUDE_DIR}")
endif() 