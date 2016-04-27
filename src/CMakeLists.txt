# @author  Barthélémy von Haller
# @author  Adam Wegrzynek

set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Monitoring/Version.h" # Generated header  
  ../include/Monitoring/MonInfoLogger.h
  ../include/Monitoring/ApMonBackend.h
  ../include/Monitoring/Backend.h
  ../include/Monitoring/Collector.h
  ../include/Monitoring/Metric.h
  ../include/Monitoring/StdoutBackend.h
  ../include/Monitoring/TemplatedMetric.h
)

set(LIB_SOURCES
  ApMonBackend.cpp
  Collector.cpp
  Metric.cpp
  StdoutBackend.cpp
  TemplatedMetric.cpp
)

if(APMON_FOUND)
	message(STATUS "  Adding ApMon classes...")
	list(APPEND LIB_HEADERS ../include/Monitoring/ApMonBackend.h)
	list(APPEND LIB_SOURCES ApMonBackend.cpp)
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
target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} InfoLogger-static Configuration-static apmoncpp)
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
