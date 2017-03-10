# @author  Barthélémy von Haller
# @author  Adam Wegrzynek

set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Monitoring/Version.h" # Generated header  
  ../include/Monitoring/Backend.h
  ../include/Monitoring/Collector.h
  ../include/Monitoring/Metric.h
  ../include/Monitoring/DerivedMetrics.h
  ../include/Monitoring/ProcessMonitor.h
  ../include/Monitoring/MonitoringFactory.h
  ../include/Monitoring/Tag.h
)

set(LIB_SOURCES
  Collector.cxx
  Metric.cxx
  Backends/InfoLoggerBackend.cxx
  Backends/Flume.cxx
  DerivedMetrics.cxx
  ProcessMonitor.cxx
  ProcessDetails.cxx
  MonitoringFactory.cxx
  Transports/UDP.cxx
  Transports/HTTP.cxx
  Exceptions/MonitoringException.cxx
  Exceptions/MonitoringInternalException.cxx
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

find_package(APMON)
if(APMON_FOUND)
  message(STATUS "  Compiling ApMon backend")
  include_directories(${APMON_INCLUDE_DIRS}) 
  list(APPEND LIBS ${APMON_LIBRARIES})
  list(APPEND LIB_SOURCES Backends/ApMonBackend.cxx)
  add_definitions(-D_WITH_APPMON)
else()
  message(WARNING "  ApMon not found, skipping ApMon backend")
endif()

find_package(CURL)
if(CURL_FOUND)
  message(STATUS "  Compiling InfluxDB HTTP and UDP backend")
  add_definitions(-D_WITH_INFLUX)
  list(APPEND LIB_SOURCES Backends/InfluxDB.cxx)
else()
  message(WARNING "  libcurl not found, skipping InfluxDB backend")
endif()

set(APP_HEADERS
)

set(APP_SOURCES
)

# libDataCollector
set(LIBRARY_NAME_DATACOLLECTOR DataCollector)
set(INCLUDE include/Monitoring)
add_library(${LIBRARY_NAME_DATACOLLECTOR} SHARED ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} Common InfoLogger-static ${Configuration_LIBRARIES} ${Boost_SYSTEM_LIBRARY})
if(APMON_FOUND)
	target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} apmoncpp)
endif()
if(CURL_FOUND)
	target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} curl)
endif()
set_target_properties(${LIBRARY_NAME_DATACOLLECTOR} PROPERTIES
        VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
        FRAMEWORK FALSE
        PUBLIC_HEADER "${LIB_HEADERS}"
)

# Says how and where to install software
install(TARGETS ${LIBRARY_NAME_DATACOLLECTOR} ${APPLICATION_STDOUT_SAMPLE} ${APPLICATION_APMON_SAMPLE}
  EXPORT ${PROJECT_NAME}Targets            # for downstream dependencies
  ARCHIVE DESTINATION lib COMPONENT libs   # static lib
  RUNTIME DESTINATION bin COMPONENT libs   # binaries
  LIBRARY DESTINATION lib COMPONENT libs   # shared lib
  PUBLIC_HEADER DESTINATION ${INCLUDE} COMPONENT dev   # headers (note the different component -> different package)
  FRAMEWORK DESTINATION bin COMPONENT libs # for mac
)
