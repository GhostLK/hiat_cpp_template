# Automatically create compile_commands.json symlink in project root directory for clangd

function(setup_clangd_symlink)
    # Check if compile_commands.json export is enabled
    if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        message(WARNING "CMAKE_EXPORT_COMPILE_COMMANDS is not enabled. Skipping clangd symlink setup.")
        return()
    endif()
    
    # Only create symlink on Unix/Linux systems
    if(NOT UNIX)
        message(STATUS "Skipping clangd symlink setup on non-Unix system")
        return()
    endif()
    
    # Get the path to compile_commands.json in current build directory
    set(COMPILE_COMMANDS_SOURCE "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(COMPILE_COMMANDS_LINK "${CMAKE_SOURCE_DIR}/compile_commands.json")
    
    # Check if compile_commands.json exists in build directory
    if(EXISTS "${COMPILE_COMMANDS_SOURCE}")
        # Remove existing symlink if it exists
        if(EXISTS "${COMPILE_COMMANDS_LINK}")
            if(IS_SYMLINK "${COMPILE_COMMANDS_LINK}")
                file(REMOVE "${COMPILE_COMMANDS_LINK}")
            else()
                message(WARNING "compile_commands.json already exists at ${COMPILE_COMMANDS_LINK} and is not a symlink")
                return()
            endif()
        endif()
        
        # Create symlink
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink 
            "${COMPILE_COMMANDS_SOURCE}" 
            "${COMPILE_COMMANDS_LINK}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE SYMLINK_RESULT
        )
        
        if(SYMLINK_RESULT EQUAL 0)
            message(STATUS "Created compile_commands.json symlink for clangd")
        else()
            message(WARNING "Failed to create compile_commands.json symlink")
        endif()
    else()
        message(STATUS "compile_commands.json not found in build directory, skipping symlink creation")
    endif()
endfunction()

setup_clangd_symlink()