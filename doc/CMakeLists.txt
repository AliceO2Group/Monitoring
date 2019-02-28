# @author  Barthélémy von Haller
# @author  Adam Wegrzynek

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/doc/images/o2_logo.png")

  doxygen_add_docs(doc
    ${CMAKE_SOURCE_DIR}
    COMMENT "Generating doxygen documentation for ${PROJECT_NAME}"
  )

endif (DOXYGEN_FOUND)
