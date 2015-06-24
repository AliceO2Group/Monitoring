# - Try to find ACT include dirs and libraries
# Author: Barthelemy von Haller
#
# This script will set the following variables:
#  ACT_FOUND - System has ACT
#  ACT_INCLUDE_DIRS - The ACT include directories
#  ACT_LIBRARIES - The libraries needed to use ACT
#  ACT_DEFINITIONS - Compiler switches required for using ACT

# Init
include(FindPackageHandleStandardArgs)

# find includes
find_path(APMON_INCLUDE_DIR ApMon.h
          PATHS /usr/local/include)
set(APMON_INCLUDE_DIRS ${APMON_INCLUDE_DIR} )

# find library
find_library(APMON_LIBRARY NAMES apmoncpp HINTS /usr/local )
set(APMON_LIBRARIES ${APMON_LIBRARY} )

# handle the QUIETLY and REQUIRED arguments and set ACT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(APMON  DEFAULT_MSG
                                  APMON_LIBRARY APMON_INCLUDE_DIR)

mark_as_advanced(APMON_INCLUDE_DIR APMON_LIBRARY )
