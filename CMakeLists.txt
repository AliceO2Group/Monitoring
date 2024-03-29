# Copyright 2019-2020 CERN and copyright holders of ALICE O2.
# See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
# All rights not expressly granted are reserved.
#
# This software is distributed under the terms of the GNU General Public
# License v3 (GPL Version 3), copied verbatim in the file "COPYING".
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization
# or submit itself to any jurisdiction.
#
# Author Adam Wegrzynek <adam.wegrzynek@cern.ch>


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
  VERSION 3.18.2
  DESCRIPTION "O2 Monitoring library"
  LANGUAGES CXX
)

# Documentation dir
add_subdirectory(doc)

# simplified build mode for doc only
if(ONLYDOC)
  return()
endif()

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

find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework program_options system)
find_package(ApMon MODULE)
find_package(CURL MODULE)
find_package(RdKafka CONFIG)
find_package(InfoLogger CONFIG)
find_package(Protobuf CONFIG)
find_package(gRPC CONFIG)

####################################
# Set OUTPUT vars
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

####################################
# Handle RPATH
####################################
include(GNUInstallDirs)

# Use relative RPATHs
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
  endif()
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
  endif()
endif()
unset(isSystemDir)

####################################
# Library
####################################

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
  src/Transports/WebSocket.cxx
  src/Exceptions/MonitoringException.cxx
  $<$<BOOL:${ApMon_FOUND}>:src/Backends/ApMonBackend.cxx>
  $<$<BOOL:${RdKafka_FOUND}>:src/Transports/KafkaProducer.cxx>
  $<$<BOOL:${RdKafka_FOUND}>:src/Transports/KafkaConsumer.cxx>
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
    $<$<BOOL:${RdKafka_FOUND}>:RdKafka::rdkafka++>
  PRIVATE
    Boost::system
    pthread
    $<$<BOOL:${ApMon_FOUND}>:ApMon::ApMon>
    $<$<BOOL:${CURL_FOUND}>:CURL::libcurl>
    $<$<BOOL:${InfoLogger_FOUND}>:AliceO2::InfoLogger>
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
  # Detecto CC7 or CS8
  if(EXISTS "/etc/os-release")
    file(STRINGS "/etc/os-release" OS_RELEASE)
    foreach(KV ${OS_RELEASE})
      if(KV MATCHES "^VERSION=\"8\"")
        set(CS8 true)
        message(STATUS "  Detected CS8")
      endif()
      if(KV MATCHES "^VERSION=\"7")
        set(CC7 true)
        message(STATUS "  Detected CC7")
      endif()
    endforeach()
  endif()
endif()

if (APPLE)
  message(STATUS "Detected macOS: Process monitor disabled")
endif()

if(Protobuf_FOUND AND RdKafka_FOUND)
   message(STATUS "Compiling Kafka consumer with protobuf deserializer")
endif()

# Handle custom compile definitions
target_compile_definitions(Monitoring
  PRIVATE
    $<$<BOOL:${APPLE}>:O2_MONITORING_OS_MAC>
    $<$<BOOL:${LINUX}>:O2_MONITORING_OS_LINUX>
    $<$<BOOL:${CC7}>:O2_MONITORING_OS_CC7>
    $<$<BOOL:${CS8}>:O2_MONITORING_OS_CS8>
    $<$<BOOL:${ApMon_FOUND}>:O2_MONITORING_WITH_APPMON>
    $<$<BOOL:${RdKafka_FOUND}>:O2_MONITORING_WITH_KAFKA>
    $<$<BOOL:${CURL_FOUND}>:O2_MONITORING_WITH_CURL>
    $<$<BOOL:${InfoLogger_FOUND}>:O2_MONITORING_WITH_INFOLOGGER>
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
  examples/9-RunNumber.cxx
  examples/10-Buffering.cxx
)

if(RdKafka_FOUND)
  list(APPEND EXAMPLES "examples/11-KafkaToWebsocket.cxx")
  list(APPEND EXAMPLES "examples/13-PullClient.cxx")
endif()

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

if(RdKafka_FOUND)
  set_target_properties(11-KafkaToWebsocket PROPERTIES OUTPUT_NAME "o2-monitoring-kafka-to-grafana")
endif()


####################################
# Generate protobuf
####################################
if(RdKafka_FOUND AND Protobuf_FOUND AND CURL_FOUND)
  set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/envs.proto)
  get_filename_component(PROTO_OUTPUT_NAME ${PROTO_FILE} NAME_WE)
  get_filename_component(PROTO_FILE_PREFIX ${PROTO_FILE} PATH)
  set(PROTO_CPP_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_OUTPUT_NAME}.pb.cc)

  add_custom_command(
      OUTPUT "${PROTO_CPP_OUTPUT}"
      COMMAND protobuf::protoc
      ARGS --proto_path ${PROTO_FILE_PREFIX} --cpp_out ${CMAKE_CURRENT_BINARY_DIR} ${PROTO_FILE}
      DEPENDS ${PROTO_FILE}
      COMMENT "Running protoc on ${PROTO_FILE}"
      VERBATIM)

  set(PROTO_EXAMPLES
    examples/12-KafkaToInfluxDb.cxx
    examples/14-OrbitId.cxx
    examples/8-KafkaToHttpServer.cxx
  )
  foreach (example ${PROTO_EXAMPLES})
    get_filename_component(example_name ${example} NAME)
    string(REGEX REPLACE ".cxx" "" example_name ${example_name})
    add_executable(${example_name} ${example} ${PROTO_CPP_OUTPUT})
    target_link_libraries(${example_name} PRIVATE
      Monitoring
      Boost::program_options
      protobuf::libprotobuf
      $<$<BOOL:${InfoLogger_FOUND}>:AliceO2::InfoLogger>
    )
    target_compile_definitions(${example_name} PRIVATE $<$<BOOL:${InfoLogger_FOUND}>:O2_MONITORING_WITH_INFOLOGGER>)
    target_include_directories(${example_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS ${example_name})
  endforeach()
  set_target_properties(8-KafkaToHttpServer PROPERTIES OUTPUT_NAME "o2-monitoring-env-webserver")
  set_target_properties(12-KafkaToInfluxDb PROPERTIES OUTPUT_NAME "o2-monitoring-kafka-to-influxdb")
  set_target_properties(14-OrbitId PROPERTIES OUTPUT_NAME "o2-monitoring-orbitid")
endif()

####################################
# gRPC
####################################
if(Protobuf_FOUND AND gRPC_FOUND)
  set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/odc.proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/o2control.proto)
  set(EXAMPLES examples/15-ODC.cxx examples/16-AliECS.cxx)
  foreach(PROTO_FILE example IN ZIP_LISTS PROTO_FILES EXAMPLES)
    get_filename_component(PROTO_OUTPUT_NAME ${PROTO_FILE} NAME_WE)
    get_filename_component(PROTO_FILE_PREFIX ${PROTO_FILE} PATH)
    set(PROTO_CPP_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_OUTPUT_NAME}.pb.cc)
    set(GRPC_CPP_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_OUTPUT_NAME}.grpc.pb.cc)

    add_custom_command(
        OUTPUT "${PROTO_CPP_OUTPUT}"
        COMMAND protobuf::protoc
        ARGS --proto_path ${PROTO_FILE_PREFIX}
             --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
             ${PROTO_OUTPUT_NAME}.proto
        DEPENDS ${PROTO_FILE}
        COMMENT "Running protoc on ${PROTO_FILE}"
        VERBATIM)

    add_custom_command(
        OUTPUT "${GRPC_CPP_OUTPUT}"
        COMMAND protobuf::protoc
        ARGS --proto_path ${PROTO_FILE_PREFIX}
             --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
             --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
             ${PROTO_OUTPUT_NAME}.proto
        DEPENDS ${PROTO_FILE}
        COMMENT "Running protoc/gRPC on ${PROTO_FILE}"
        VERBATIM)

    get_filename_component(example_name ${example} NAME)
    string(REGEX REPLACE ".cxx" "" example_name ${example_name})
    add_executable(${example_name} ${example} ${PROTO_CPP_OUTPUT} ${GRPC_CPP_OUTPUT})
    target_link_libraries(${example_name} PRIVATE
        Monitoring
        gRPC::grpc++
        protobuf::libprotobuf
        Boost::program_options
    )
    target_include_directories(${example_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  endforeach()
  set_target_properties(15-ODC PROPERTIES OUTPUT_NAME "o2-monitoring-odc")
  set_target_properties(16-AliECS PROPERTIES OUTPUT_NAME "o2-monitoring-aliecs-tasks")
endif()

####################################
# Tests
####################################

enable_testing()

set(TEST_SRCS
  test/testMonitoringFactory.cxx
  test/testDerived.cxx
  test/testMetric.cxx
  test/testProcessDetails.cxx
  test/testProcessMonitor.cxx
  test/testRegexMatch.cxx
  test/testMonitoring.cxx
  test/testVerbosity.cxx
)

if(ApMon_FOUND)
  configure_file(test/ApMon.conf ${CMAKE_CURRENT_BINARY_DIR}/ApMon.conf COPYONLY)
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
if(RdKafka_FOUND)
  install(TARGETS 11-KafkaToWebsocket)
endif()


if(RdKafka_FOUND AND Protobuf_FOUND)
  install(TARGETS 14-OrbitId)
endif()

if(Protobuf_FOUND AND gRPC_FOUND)
  install(TARGETS 15-ODC)
endif()

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
