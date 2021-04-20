# @author  Barthélémy von Haller
# @author  Adam Wegrzynek
# @author  Sylvain Chapeland

# define doc build path
set(DOC_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc-build")

# define doc installation path
# ensures it ends with project name
if (NOT CMAKE_INSTALL_DOCDIR)
  set(CMAKE_INSTALL_DOCDIR "doc")
endif()
if (NOT "${CMAKE_INSTALL_DOCDIR}" MATCHES "${PROJECT_NAME}$")
  set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DOCDIR}/${PROJECT_NAME}")
endif()

# log doc build/install paths
message(STATUS "Documentation will be built in ${DOC_BUILD_DIR}")
message(STATUS "Documentation will be installed in ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}")

# doxygen-generated doc
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (DOXYGEN_FOUND)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/doc/images/o2_logo.png")
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
  set(DOXYGEN_HAVE_DOT YES)
  set(DOXYGEN_DOT_NUM_THREADS 1)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
  set(DOXYGEN_TEMPLATE_RELATIONS YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT svg)
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
  set(DOXYGEN_DOT_TRANSPARENT YES)
  set(DOXYGEN_OUTPUT_DIRECTORY ${DOC_BUILD_DIR})
  
  doxygen_add_docs(doc
    ${CMAKE_SOURCE_DIR}
    COMMENT "Generating doxygen documentation for ${PROJECT_NAME}"
  )

  # install generated files
  install(
    DIRECTORY ${DOC_BUILD_DIR}/html
    TYPE DOC
    OPTIONAL # because available only after "make doc"
  )

endif (DOXYGEN_FOUND)


# if needed add other doc files from source tree
# install(
#  DIRECTORY . 
#  TYPE DOC
#  FILES_MATCHING PATTERN "*.md" "*.png"
#)
