set(memcheck-cover_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/memcheck-cover)

function(AddMemcheck target)
  set(MEMCHECK_PATH ${memcheck-cover_SOURCE_DIR}/bin)
  set(REPORT_PATH "${CMAKE_BINARY_DIR}/valgrind-${target}")

  add_custom_target(memcheck-${target} # memcheck-svc_test
    COMMAND ${MEMCHECK_PATH}/memcheck_runner.sh -o
            "${REPORT_PATH}/report"
            -- $<TARGET_FILE:${target}>
    COMMAND ${MEMCHECK_PATH}/generate_html_report.sh
            -i ${REPORT_PATH}
            -o ${REPORT_PATH}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endfunction()
