# @author  Barthélémy von Haller
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
  main.cxx
)

# LIBRARIES

# libDataCollector
set(LIBRARY_NAME_DATACOLLECTOR DataCollector)
set(INCLUDE include/${LIBRARY_NAME_DATACOLLECTOR})
add_library(${LIBRARY_NAME_DATACOLLECTOR} SHARED ${LIB_HEADERS} ${LIB_SOURCES})
set_target_properties(${LIBRARY_NAME_DATACOLLECTOR} PROPERTIES
        VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
        FRAMEWORK FALSE
        PUBLIC_HEADER "${LIB_HEADERS}"
)



# BINARIES

# Main application
set(APPLICATION samplecollector)
add_executable(${APPLICATION} ${APP_SOURCES})
target_link_libraries(${APPLICATION} ${LIBRARY_NAME_DATACOLLECTOR} apmoncpp ${Boost_PROGRAM_OPTIONS_LIBRARY})
install(TARGETS ${APPLICATION} DESTINATION bin COMPONENT apps)



# Says how and where to install software
install(TARGETS ${LIBRARY} ${APPLICATION}
  EXPORT ${PROJECT_NAME}Targets            # for downstream dependencies
  ARCHIVE DESTINATION lib COMPONENT libs   # static lib
  RUNTIME DESTINATION bin COMPONENT libs   # binaries
  LIBRARY DESTINATION lib COMPONENT libs   # shared lib
  PUBLIC_HEADER DESTINATION ${INCLUDE} COMPONENT dev   # headers (note the different component -> different package)
  FRAMEWORK DESTINATION bin COMPONENT libs # for mac
)