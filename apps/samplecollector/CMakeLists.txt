# @author  Barthélémy von Haller

set(HEADERS
)

set(SOURCES
  main.cxx
)

# Executable name then used to add an executable to the project.
# This is a make target, so you can do a "make samplecollector"
set(APPLICATION samplecollector)
add_executable(${APPLICATION} ${SOURCES})

# Links the target exe (samplecollector) against the library (libdatacollector)
target_link_libraries(${APPLICATION} DataCollector apmoncpp ${Boost_PROGRAM_OPTIONS_LIBRARY})

# Says how and where to install software
install(TARGETS ${APPLICATION} DESTINATION bin COMPONENT apps)
