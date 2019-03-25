# - Try to find ApMon include dirs and libraries
# Author: Barthelemy von Haller
# Author: Adam Wegrzynek
#
# This script will set the following variables:
#  ApMon_FOUND - System has ApMon
#  ApMon_INCLUDE_DIRS - The ApMon include directories
#  ApMon_LIBRARIES - The libraries needed to use ApMon
# This script can use the following variables:
#  APMON_ROOT - Installation root to tell this module where to look.
# This script defines following targets:
#  ApMon::ApMon

# Init
include(FindPackageHandleStandardArgs)

# find includes
find_path(ApMon_INCLUDE_DIR ApMon.h
  HINTS ${APMON_ROOT} /usr/local
  PATH_SUFFIXES include
)

mark_as_advanced(ApMon_INCLUDE_DIR)

# find library
find_library(ApMon_LIBRARY NAMES apmoncpp
  HINTS ${APMON_ROOT} /usr/local
  PATH_SUFFIXES lib lib64
)

mark_as_advanced(ApMon_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set ApMon_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ApMon "ApMon could not be found. Install package ApMon_cpp."
        ApMon_LIBRARY ApMon_INCLUDE_DIR)

if(ApMon_FOUND)
  set(ApMon_LIBRARIES ${ApMon_LIBRARY})
  set(ApMon_INCLUDE_DIRS ${ApMon_INCLUDE_DIR})

  # add target
  if(NOT TARGET ApMon::ApMon)
    add_library(ApMon::ApMon INTERFACE IMPORTED)
    set_target_properties(ApMon::ApMon PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${ApMon_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${ApMon_LIBRARY}"
    )
  endif()
endif()
