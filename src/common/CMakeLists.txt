add_library(common "")

# =========================
# Generate error_code.h/.cpp
# =========================
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/common/include/common/error_code.h ${CMAKE_SOURCE_DIR}/src/common/error_code.c
    COMMAND ${CMAKE_COMMAND} -E echo "Generating error_code.h and error_code.c from error_codes.json"
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/scripts/python/
            python3 ${CMAKE_SOURCE_DIR}/scripts/python/gen_error_code.py
    DEPENDS
        ${CMAKE_SOURCE_DIR}/config/error_codes.json
        ${CMAKE_SOURCE_DIR}/scripts/python/gen_error_code.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/python
    COMMENT "Auto-generating error_code.h and error_code.c"
)
add_custom_target(
    gen_error_code
    DEPENDS
        ${CMAKE_SOURCE_DIR}/src/common/include/common/error_code.h
        ${CMAKE_SOURCE_DIR}/src/common/error_code.c
)

add_dependencies(common gen_error_code)

target_sources(common
    PRIVATE
        error_code.c
)
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(common
    PUBLIC
        svc
)
