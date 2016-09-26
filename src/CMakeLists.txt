# @author  Barthélémy von Haller
# @author  Adam Wegrzynek

set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Monitoring/Version.h" # Generated header  
  ../include/Monitoring/Backend.h
  ../include/Monitoring/Collector.h
  ../include/Monitoring/Metric.h
  ../include/Monitoring/DerivedMetrics.h
  ../include/Monitoring/ProcessMonitor.h
)

set(LIB_SOURCES
  Collector.cpp
  Metric.cpp
  InfoLoggerBackend.cpp
  DerivedMetrics.cpp
  ProcessMonitor.cpp
)

if(APMON_FOUND)
	message(STATUS "  Adding ApMon backend classes...")
	list(APPEND LIB_SOURCES ApMonBackend.cpp)
endif()
FIND_PACKAGE(CURL)
if(CURL_FOUND)
        message(STATUS "libcurl found")
	message(STATUS "  Compiling InfluxDB Backend")
	add_definitions(-D_WITH_INFLUX)
        list(APPEND LIB_SOURCES InfluxBackend.cpp)
endif()


set(APP_HEADERS
)

set(APP_SOURCES
)

# LIBRARIES

# libDataCollector
set(LIBRARY_NAME_DATACOLLECTOR DataCollector)
set(INCLUDE include/Monitoring)
add_library(${LIBRARY_NAME_DATACOLLECTOR} SHARED ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} InfoLogger-static Configuration-static)
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
