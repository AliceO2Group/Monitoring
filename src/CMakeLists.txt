# @author  Barthélémy von Haller
# @author  Vasco Barroso
set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Monitoring/Version.h" # Generated header  
  ../include/Monitoring/DataCollectorInterface.h
  ../include/Monitoring/DataCollector.h
  ../include/Monitoring/DataCollectorApMon.h
)

set(LIB_SOURCES
  DataCollectorApMon.cxx
  DataCollector.cxx
)

set(APP_HEADERS
)

set(APP_SOURCES
)

# LIBRARIES

# libDataCollector
set(LIBRARY_NAME_DATACOLLECTOR DataCollector)
set(INCLUDE include/${LIBRARY_NAME_DATACOLLECTOR})
add_library(${LIBRARY_NAME_DATACOLLECTOR} SHARED ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIBRARY_NAME_DATACOLLECTOR} InfoLogger-static)
set_target_properties(${LIBRARY_NAME_DATACOLLECTOR} PROPERTIES
        VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
        FRAMEWORK FALSE
        PUBLIC_HEADER "${LIB_HEADERS}"
)



# BINARIES

# example writing to stdout
set(APPLICATION_STDOUT_SAMPLE sampleCollectorStdout)
add_executable(${APPLICATION_STDOUT_SAMPLE} sampleCollectorStdout)
target_link_libraries(${APPLICATION_STDOUT_SAMPLE} ${LIBRARY_NAME_DATACOLLECTOR} apmoncpp ${Boost_PROGRAM_OPTIONS_LIBRARY})
install(TARGETS ${APPLICATION_STDOUT_SAMPLE} DESTINATION bin COMPONENT apps)

# example with ApMon
set(APPLICATION_APMON_SAMPLE sampleCollectorApMon)
add_executable(${APPLICATION_APMON_SAMPLE} sampleCollectorApMon)
target_link_libraries(${APPLICATION_APMON_SAMPLE} ${LIBRARY_NAME_DATACOLLECTOR} apmoncpp ${Boost_PROGRAM_OPTIONS_LIBRARY})
install(TARGETS ${APPLICATION_APMON_SAMPLE} DESTINATION bin COMPONENT apps)



# Says how and where to install software
install(TARGETS ${LIBRARY_NAME_DATACOLLECTOR} ${APPLICATION_STDOUT_SAMPLE} ${APPLICATION_APMON_SAMPLE}
  EXPORT ${PROJECT_NAME}Targets            # for downstream dependencies
  ARCHIVE DESTINATION lib COMPONENT libs   # static lib
  RUNTIME DESTINATION bin COMPONENT libs   # binaries
  LIBRARY DESTINATION lib COMPONENT libs   # shared lib
  PUBLIC_HEADER DESTINATION ${INCLUDE} COMPONENT dev   # headers (note the different component -> different package)
  FRAMEWORK DESTINATION bin COMPONENT libs # for mac
)