add_executable(1-Basic 1-Basic.cxx)
target_link_libraries(1-Basic DataCollector)

add_executable(2-UserDefinedEntity 2-UserDefinedEntity.cxx)
target_link_libraries(2-UserDefinedEntity DataCollector)

add_executable(3-UserDefinedTimestamp 3-UserDefinedTimestamp.cxx)
target_link_libraries(3-UserDefinedTimestamp DataCollector)

add_executable(4-RateDerivedMetric 4-RateDerivedMetric.cxx)
target_link_libraries(4-RateDerivedMetric DataCollector)

add_executable(5-MonitorAdditionalPid 5-MonitorAdditionalPid.cxx)
target_link_libraries(5-MonitorAdditionalPid DataCollector)
