include(Coverage)
include(Doxygen)

add_subdirectory(app)
add_subdirectory(svc)

Doxygen(tracker  src/tracker docs-tmp)

# ============================================================================
# Manage dependencies
# ============================================================================
include(nlohmann_json)
include(spdlog)

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

target_link_libraries(seeker
    PRIVATE
        svc
)

# ============================================================================
# TODO: Installation Configuration
# ============================================================================

# Install systemd service file
install(FILES ${CMAKE_SOURCE_DIR}/scripts/app.service
        DESTINATION /etc/systemd/system
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# Install main program
install(TARGETS seeker
        RUNTIME DESTINATION /opt/app)

# ============================================================================
# TODO: Packaging Configuration
# ============================================================================

# Basic package information
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "tracker-test-server")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "lukeplusplus@163.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tracker Test Server with Heartbeat Monitor")
set(CPACK_PACKAGE_DESCRIPTION "A comprehensive tracker test server with built-in heartbeat monitoring,
automatic restart capabilities, and UDP/TCP communication support.
Includes FFmpeg integration for video streaming.")

# Debian specific configuration
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "HIAT <lukeplusplus@163.com>")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "net")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")

# Package management scripts - point to the correct scripts directory
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/scripts/preinst;${CMAKE_SOURCE_DIR}/scripts/postinst;${CMAKE_SOURCE_DIR}/scripts/prerm;${CMAKE_SOURCE_DIR}/scripts/postrm")

# File permission configuration
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)

# Conflict and replacement configuration
set(CPACK_DEBIAN_PACKAGE_CONFLICTS "tracker-server, tracker-test")
set(CPACK_DEBIAN_PACKAGE_PROVIDES "tracker-test-server")

# Package metadata
set(CPACK_PACKAGE_VENDOR "HIAT")

# Package file name format
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
