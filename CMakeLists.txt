####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.

FOREACH (p
        CMP0025 # CMake 3.0
        CMP0042 # CMake 3.0
        )
    IF (POLICY ${p})
        cmake_policy(SET ${p} NEW)
    ENDIF ()
endforeach ()

# Define project
project(Monitoring
  VERSION 1.7.0
  DESCRIPTION "O2 Monitoring library"
  LANGUAGES CXX
)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE
    STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE
  )
endif(NOT CMAKE_BUILD_TYPE)

# Documentation dir
add_subdirectory(doc)

# C++14
set(CMAKE_CXX_STANDARD 14)

# Add compiler flags for warnings and (more importantly) fPIC and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -fPIC")

####################################
# Dependencies
####################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost REQUIRED COMPONENTS unit_test_framework program_options system)
find_package(Git QUIET)
find_package(ApMon MODULE)
find_package(CURL REQUIRED MODULE)

####################################
# Library
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")


set(SRCS
  src/Monitoring.cxx
  src/Metric.cxx
  src/Backends/InfoLoggerBackend.cxx
  src/Backends/InfluxDB.cxx
  src/Backends/Flume.cxx
  src/Backends/StdOut.cxx
  src/DerivedMetrics.cxx
  src/ProcessMonitor.cxx
  src/ProcessDetails.cxx
  src/MonitoringFactory.cxx
  src/Transports/UDP.cxx
  src/Transports/TCP.cxx
  src/Transports/HTTP.cxx
  src/Exceptions/MonitoringException.cxx
  src/Exceptions/MonitoringInternalException.cxx
)

# Detect operating system
if (UNIX AND NOT APPLE)
    message(STATUS "Detected Linux OS: Process monitor enabled")
    add_definitions(-D_OS_LINUX)
endif()

if (WIN32)
    message(STATUS "Detected Windows OS: Process monitor disabled")
    add_definitions(-D_OS_WINDOWS)
endif()

if (APPLE)
    message(STATUS "Detected Mac OS: Process monitor disabled")
    add_definitions(-D_OS_MAC)
endif()

# Backends
message(STATUS "Backends")
message(STATUS "  Compiling InfoLoggerBackend backend")
message(STATUS "  Compiling Flume UDP/JSON backend")
message(STATUS "  Compiling InfluxDB HTTP and UDP backend")

if(ApMon_FOUND)
    message(STATUS "  Compiling ApMon backend")
    list(APPEND SRCS src/Backends/ApMonBackend.cxx)
    add_definitions(-D_WITH_APPMON)
else()
    message(STATUS "  ApMon not found, skipping ApMon backend")
endif()

add_library(Monitoring SHARED ${SRCS})

target_include_directories(Monitoring
  PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Monitoring
  PUBLIC
    Boost::boost
  PRIVATE
    Boost::system CURL::CURL
)

# Handle optional ApMon dependency
if(ApMon_FOUND)
  target_link_libraries(Monitoring
    PRIVATE
      ApMon::ApMon
  )
endif()

####################################
# Executables
####################################

set(EXAMPLES
  examples/1-Basic.cxx
  examples/2-TaggedMetrics.cxx
  examples/3-UserDefinedTimestamp
  examples/4-RateDerivedMetric.cxx
  examples/5-Benchmark.cxx
  examples/6-Increment.cxx
  examples/7-Latency.cxx
  examples/8-Multiple.cxx
  examples/9-Timer.cxx
  examples/10-Buffering.cxx
  examples/11-AutoUpdate.cxx
)

foreach (example ${EXAMPLES})
  get_filename_component(example_name ${example} NAME)
  string(REGEX REPLACE ".cxx" "" example_name ${example_name})
  add_executable(${example_name} ${example})
  target_link_libraries(${example_name}
    PRIVATE
      Monitoring Boost::program_options
  )
endforeach()

####################################
# Tests
####################################

enable_testing()

set(TEST_SRCS
  test/testMonitoring.cxx
  test/testMonitoringFactory.cxx
  test/testDerived.cxx
  test/testFlume.cxx
  test/testMetric.cxx
  test/testProcessDetails.cxx
  test/testProcessMonitor.cxx
  test/testInfluxDb.cxx
  test/testNoop.cxx
)

if(ApMon_FOUND)
  list(APPEND TEST_SRCS test/testApMon.cxx)
  configure_file(test/ApMon.conf ApMon.conf COPYONLY)
endif()

foreach (test ${TEST_SRCS})
  get_filename_component(test_name ${test} NAME)
  string(REGEX REPLACE ".cxx" "" test_name ${test_name})

  add_executable(${test_name} ${test})
  target_link_libraries(${test_name}
    PRIVATE 
      Monitoring Boost::unit_test_framework
  )
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 60)
endforeach()
