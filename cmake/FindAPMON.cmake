# - Try to find ApMon include dirs and libraries
# Author: Barthelemy von Haller
#
# This script will set the following variables:
#  APMON_FOUND - System has ApMon
#  APMON_INCLUDE_DIRS - The ApMon include directories
#  APMON_LIBRARIES - The libraries needed to use ApMon
#  APMON_DEFINITIONS - Compiler switches required for using ApMon

# Init
include(FindPackageHandleStandardArgs)

# find includes
find_path(APMON_INCLUDE_DIR ApMon.h
          PATHS /usr/local/include)
set(APMON_INCLUDE_DIRS ${APMON_INCLUDE_DIR})

# find library
find_library(APMON_LIBRARY NAMES apmoncpp HINTS /usr/local)
set(APMON_LIBRARIES ${APMON_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set APMON_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(APMON  "ApMon could not be found. Install package ApMon_cpp."
                                  APMON_LIBRARY APMON_INCLUDE_DIR)

if(APMON_FOUND)
    message(info "APMON found")
endif()

mark_as_advanced(APMON_INCLUDE_DIR APMON_LIBRARY)
