# @author  Barthélémy von Haller
# @author  Adam Wegrzynek

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
  # Configure the doxygen config file with current settings
  set("DOC_OUTPUT_DIR" "${CMAKE_CURRENT_BINARY_DIR}/html")
  configure_file(doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/documentation-config.doxygen @ONLY)

  doxygen_add_docs(doc
    ${CMAKE_CURRENT_BINARY_DIR}/documentation-config.doxygen
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating doxygen documentation for ${PROJECT_NAME}: ${DOC_OUTPUT_DIR}"
  )
endif (DOXYGEN_FOUND)
