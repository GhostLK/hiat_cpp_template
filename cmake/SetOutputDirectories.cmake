# Set the output directories for the project

include(GNUInstallDirs)

# Check if the generator is multi-config
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(IS_MULTI_CONFIG)
    # Multi-config generator, use generator expression
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_INSTALL_BINDIR})
    
    # Windows special handling: DLLs need to be in the bin directory
    if(WIN32)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_INSTALL_BINDIR})
    endif()
    
    message(STATUS "Multi-config generator detected, using config subdirectories")
    
    # Create directories for common configurations
    foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CONFIG}/${CMAKE_INSTALL_LIBDIR})
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CONFIG}/${CMAKE_INSTALL_BINDIR})
    endforeach()
    
else()
    # Single-config generator, force use of config subdirectories
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()
    
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_BINDIR})
    
    # Windows special handling
    if(WIN32)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_BINDIR})
    endif()
    
    message(STATUS "Single-config generator with forced config subdirs, build type: ${CMAKE_BUILD_TYPE}")
    
    # Create directories only for the current build type
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_LIBDIR})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_BINDIR})
endif()

# Print output directory information
message(STATUS "Output directories configured:")
message(STATUS "  Archives (static libs): ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "  Libraries (shared libs): ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "  Runtime (executables): ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}") 