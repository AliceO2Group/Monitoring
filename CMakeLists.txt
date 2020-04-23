####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

# Set cmake policy by version: https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

# Define project
project(Monitoring
  VERSION 3.0.6
  DESCRIPTION "O2 Monitoring library"
  LANGUAGES CXX
)

# Documentation dir
add_subdirectory(doc)

# Add compiler flags for warnings and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_ERROR_CODE_HEADER_ONLY")

# Set fPIC for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMAKE_INSTALL_LIBDIR explicitly to lib (to avoid lib64 on CC7)
set(CMAKE_INSTALL_LIBDIR lib)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE
    STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE
  )
endif()

# Add coverage flags to Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
endif()

####################################
# Dependencies
####################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost REQUIRED COMPONENTS unit_test_framework program_options system filesystem)
find_package(Git QUIET)
find_package(ApMon MODULE)
find_package(CURL MODULE)
find_package(RdKafka CONFIG)

####################################
# Handle RPATH
####################################
include(GNUInstallDirs)

# Build targets with install rpath on Mac to dramatically speed up installation
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
  endif()
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
unset(isSystemDir)

####################################
# Library
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

# Backends
message(STATUS "Backends")
message(STATUS "  Compiling StdCout backend")
message(STATUS "  Compiling InfluxDB backend with Unix socket and UDP transport")

# Create library
add_library(Monitoring SHARED
  src/Monitoring.cxx
  src/Metric.cxx
  src/Backends/InfluxDB.cxx
  src/Backends/StdOut.cxx
  src/DerivedMetrics.cxx
  src/ProcessMonitor.cxx
  src/ProcessDetails.cxx
  src/MonitoringFactory.cxx
  src/Transports/UDP.cxx
  src/Transports/TCP.cxx
  src/Transports/Unix.cxx
  src/Transports/StdOut.cxx
  src/Exceptions/MonitoringException.cxx
  $<$<BOOL:${ApMon_FOUND}>:src/Backends/ApMonBackend.cxx>
  $<$<BOOL:${RdKafka_FOUND}>:src/Transports/Kafka.cxx>
  $<$<BOOL:${CURL_FOUND}>:src/Transports/HTTP.cxx>
)

target_include_directories(Monitoring
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Rename library
set_target_properties(Monitoring PROPERTIES OUTPUT_NAME "O2Monitoring")

# Link targets
target_link_libraries(Monitoring
  PUBLIC
    Boost::boost
  PRIVATE
    Boost::system
    pthread
    $<$<BOOL:${ApMon_FOUND}>:ApMon::ApMon>
    $<$<BOOL:${RdKafka_FOUND}>:RdKafka::rdkafka++>
    $<$<BOOL:${CURL_FOUND}>:CURL::libcurl>
)

# Handle ApMon optional dependency
if(ApMon_FOUND)
  message(STATUS "  Compiling ApMon backend")
endif()

if(RdKafka_FOUND)
   message(STATUS "  Compiling Kafka transport")
endif()

if(CURL_FOUND)
   message(STATUS "  Compiling HTTP transport/InfluxDB 2.x backend")
endif()

# Detect operating system
if (UNIX AND NOT APPLE)
  message(STATUS "Detected Linux: Process monitor enabled")
  set(LINUX true)
endif()

if (APPLE)
  message(STATUS "Detected macOS: Process monitor disabled")
endif()

# Handle custom compile definitions
target_compile_definitions(Monitoring
  PRIVATE
    $<$<BOOL:${APPLE}>:O2_MONITORING_OS_MAC>
    $<$<BOOL:${LINUX}>:O2_MONITORING_OS_LINUX>
    $<$<BOOL:${ApMon_FOUND}>:O2_MONITORING_WITH_APPMON>
    $<$<BOOL:${RdKafka_FOUND}>:O2_MONITORING_WITH_KAFKA>
    $<$<BOOL:${CURL_FOUND}>:O2_MONITORING_WITH_CURL>
  )

# Use C++17
target_compile_features(Monitoring PUBLIC cxx_std_17)


####################################
# Executables
####################################

set(EXAMPLES
  examples/1-Basic.cxx
  examples/2-TaggedMetrics.cxx
  examples/3-Verbosity.cxx
  examples/4-RateDerivedMetric.cxx
  examples/5-Benchmark.cxx
  examples/6-Increment.cxx
  examples/7-InternalBenchamrk.cxx
  examples/10-Buffering.cxx
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

set_target_properties(5-Benchmark PROPERTIES OUTPUT_NAME "o2-monitoring-benchmark")

####################################
# Tests
####################################

enable_testing()

set(TEST_SRCS
  test/testMonitoring.cxx
  test/testMonitoringFactory.cxx
  test/testDerived.cxx
  test/testMetric.cxx
  test/testProcessDetails.cxx
  test/testProcessMonitor.cxx
  test/testInfluxDb.cxx
  test/testNoop.cxx
  test/testRegexMatch.cxx
)

if(ApMon_FOUND)
  list(APPEND TEST_SRCS test/testApMon.cxx)
  configure_file(test/ApMon.conf ${CMAKE_CURRENT_BINARY_DIR}/ApMon.conf COPYONLY)
endif()

foreach (test ${TEST_SRCS})
  get_filename_component(test_name ${test} NAME)
  string(REGEX REPLACE ".cxx" "" test_name ${test_name})

  add_executable(${test_name} ${test})
  target_link_libraries(${test_name}
    PRIVATE
      Monitoring Boost::unit_test_framework Boost::filesystem
  )
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 60)
endforeach()


####################################
# Install
####################################

# Install library
install(TARGETS Monitoring 5-Benchmark
  EXPORT MonitoringTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cmake/MonitoringConfigVersion.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/Monitoring DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Export targets
install(EXPORT MonitoringTargets
  FILE
    MonitoringTargets.cmake
  NAMESPACE
    AliceO2::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Monitoring
)

# Configure and install Config files
configure_package_config_file(
  cmake/MonitoringConfig.cmake.in cmake/MonitoringConfig.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Monitoring"
  PATH_VARS CMAKE_INSTALL_PREFIX
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MonitoringConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MonitoringConfigVersion.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Monitoring
)
