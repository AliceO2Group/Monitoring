####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)

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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

project(Monitoring)

# Load some basic macros which are needed later on
include(O2Utils)
include(MonitoringDependencies)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"
            CACHE
            STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# C++14
IF (CMAKE_VERSION VERSION_LESS 3.1)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++14 COMPILER_SUPPORTS_CXX14)
    if (COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else ()
        message(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
    endif ()
ELSE ()
    set(CMAKE_CXX_STANDARD 14)  # proper way in CMake >= 3.1
ENDIF ()

# Add compiler flags for warnings and (more importantly) fPIC and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -fPIC")

add_subdirectory(doc)



####################################
# Module, library and executable definition
####################################

set(MODULE_NAME "Monitoring")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/Collector.cxx
        src/Metric.cxx
        src/Backends/InfoLoggerBackend.cxx
        src/Backends/Flume.cxx
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

if(APMON_FOUND)
    message(STATUS "  Compiling ApMon backend")
    list(APPEND SRCS src/Backends/ApMonBackend.cxx)
    add_definitions(-D_WITH_APPMON)
else()
    message(WARNING "  ApMon not found, skipping ApMon backend")
endif()

if(CURL_FOUND)
    message(STATUS "  Compiling InfluxDB HTTP and UDP backend")
    add_definitions(-D_WITH_INFLUX)
    list(APPEND SRCS src/Backends/InfluxDB.cxx)
else()
    message(WARNING "  libcurl not found, skipping InfluxDB backend")
endif()

# Produce the final Version.h using template Version.h.in and substituting variables.
# We don't want to polute our source tree with it, thus putting it in binary tree.
configure_file("include/${MODULE_NAME}/Version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/${MODULE_NAME}/Version.h"
        @ONLY
        )

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/include
)

set(LIBRARY_NAME Monitoring)
set(BUCKET_NAME o2_monitoring_bucket)
O2_GENERATE_LIBRARY()

enable_testing()

set(TEST_SRCS
        test/testCollector.cxx
        test/testDerived.cxx
        test/testFlume.cxx
        test/testMetric.cxx
        test/testProcessDetails.cxx
        test/testProcessMonitor.cxx
        )

O2_GENERATE_TESTS(
        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS ${TEST_SRCS}
)

set(EXAMPLES
        examples/1-Basic.cxx
        examples/2-TaggedMetrics.cxx
        examples/3-UserDefinedTimestamp
        examples/4-RateDerivedMetric.cxx
        examples/5-Benchmark.cxx
        examples/7-Latency.cxx
        examples/8-Multiple.cxx
        )

foreach (example ${EXAMPLES})

    get_filename_component(example_name ${example} NAME)
    string(REGEX REPLACE ".cxx" "" example_name ${example_name})

    O2_GENERATE_EXECUTABLE(
            EXE_NAME ${example_name}
            SOURCES ${example}
            MODULE_LIBRARY_NAME ${LIBRARY_NAME}
            BUCKET_NAME ${BUCKET_NAME}
    )

endforeach()
