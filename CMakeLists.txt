# REFERENCE PROJECT CMAKELISTS.TXT
#
# @author  bvonhall

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake) # main (top) cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir

# Set the name of your project here
project("Monitoring" CXX)

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# Macros and variables for all O2 subprojects
include(O2)

## Git (and its revision)
find_package(Git)
# GetGitRevisionDescription module to retreive branch and revision information from Git
# Starting with Git 1.9 the module will be part of official cMake distribution, until then it has to be
# part of the application
# The Git module will trigger a reconfiguration for each pull that will bring a new revision on the local repository
set (VCS_REVISION "-1")
if(GIT_FOUND)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    message(STATUS "GIT branch ${GIT_REFSPEC}")
    message(STATUS "GIT revision ${GIT_SHA1}")
    set (VCS_REVISION ${GIT_SHA1})
endif()

# C++11 needed
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++11 COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost COMPONENTS unit_test_framework program_options REQUIRED)

find_package(APMON)
if(APMON_FOUND)
    message(STATUS "APMON ${APMON_VERSION} found")
    include_directories(${APMON_INCLUDE_DIRS})
    list(APPEND LIBS ${APMON_LIBRARIES})
else()
    message(WARNING "APMON not found, we won't compile the Monitoring module (skip, no error)")
endif()

# This sets the include directory for the reference project. This is
# the -I flag in gcc. All the includes should be in this variable To
# add new directories to the include path, just add them in this list
# (after a space), or add an include_directories statement in a
# subdirectory's CMakeLists.
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include   # add the binary tree to the search path so that we will find Version.h
    ${Boost_INCLUDE_DIRS}
)

# Mac needed variables (adapt according to your needs)
set(CMAKE_MACOSX_RPATH ON)
#set(CMAKE_SKIP_BUILD_RPATH FALSE)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(doc)

if(APMON_FOUND) # TODO remove this once we have our own repo and we ensure that APMON is available
    enable_testing() # needed on top-level CMakeLists.txt
    add_subdirectory(include)
    add_subdirectory(src)
    add_subdirectory(test)

    # PACKAGING
    # _____________________________________________________________________________

    include(CPackConfig)

    #
    # CMAKE PACKAGING (for other CMake projects to use this one easily)
    # _____________________________________________________________________________

    include(PackageConfigurator)
endif()
