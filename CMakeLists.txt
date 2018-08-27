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
find_package(ApMon)
find_package(CURL)

set(extra_deps "") 
set(extra_deps_include "") 
if (ApMon_FOUND)
    list(APPEND extra_deps ${ApMon_LIBRARIES})
    list(APPEND extra_deps_include ${ApMon_INCLUDE_DIRS})
endif ()
if (CURL_FOUND)
    list(APPEND extra_deps ${CURL_LIBRARIES})
    list(APPEND extra_deps_include ${CURL_INCLUDE_DIRS})
endif ()


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

if(ApMon_FOUND)
    message(STATUS "  Compiling ApMon backend")
    list(APPEND SRCS src/Backends/ApMonBackend.cxx)
    add_definitions(-D_WITH_APPMON)
else()
    message(STATUS "  ApMon not found, skipping ApMon backend")
endif()

if(CURL_FOUND)
    message(STATUS "  Compiling InfluxDB HTTP and UDP backend")
    add_definitions(-D_WITH_INFLUX)
    list(APPEND SRCS src/Backends/InfluxDB.cxx)
else()
    message(STATUS "  libcurl not found, skipping InfluxDB backend")
endif()

set(LIBRARY_NAME Monitoring)
add_library(${LIBRARY_NAME} SHARED ${SRCS})

target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PRIVATE src)

target_include_directories(${LIBRARY_NAME} PRIVATE ${Boost_INCLUDE_DIRS} ${extra_deps_include})
target_link_libraries(${LIBRARY_NAME} PRIVATE ${Boost_SYSTEM_LIBRARY} ${extra_deps})

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
  target_link_libraries(${example_name} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
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
  target_link_libraries(${test_name} ${LIBRARY_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 60)
endforeach()
