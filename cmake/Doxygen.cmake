find_package(Doxygen REQUIRED)

set(DOXYGEN_AWESOME_CSS_DIR ${PROJECT_SOURCE_DIR}/third_party/doxygen-awesome-css)

# 检查本地目录是否存在
if(NOT EXISTS ${DOXYGEN_AWESOME_CSS_DIR})
    message(FATAL_ERROR "doxygen-awesome-css not found at ${DOXYGEN_AWESOME_CSS_DIR}. Please download it manually for offline deployment.")
endif()

set(doxygen-awesome-css_SOURCE_DIR ${DOXYGEN_AWESOME_CSS_DIR})

function(Doxygen target input output)
  set(NAME "doxygen-${target}")
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_HTML_OUTPUT   ${PROJECT_BINARY_DIR}/${output})

  UseDoxygenAwesomeCss()
  UseDoxygenAwesomeExtensions()

  doxygen_add_docs("doxygen-${target}"
      ${PROJECT_SOURCE_DIR}/${input}
      COMMENT "Generate HTML documentation"
  )
endfunction()

macro(UseDoxygenAwesomeCss)
  set(DOXYGEN_GENERATE_TREEVIEW     YES)
  set(DOXYGEN_HAVE_DOT              YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT      svg)
  set(DOXYGEN_DOT_TRANSPARENT       YES)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET
      ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css)
endmacro()

macro(UseDoxygenAwesomeExtensions)
  set(DOXYGEN_HTML_EXTRA_FILES
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-paragraph-link.js
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-interactive-toc.js
  )

  execute_process(COMMAND doxygen -w html header.html footer.html style.css
                  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  execute_process(COMMAND sed -i "/<\\/head>/r ${PROJECT_SOURCE_DIR}/scripts/doxygen/doxygen_extra_headers" header.html
                  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  set(DOXYGEN_HTML_HEADER ${PROJECT_BINARY_DIR}/header.html)
endmacro()
