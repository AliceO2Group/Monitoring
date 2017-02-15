add_executable(1-Basic 1-Basic.cxx)
target_link_libraries(1-Basic DataCollector)

add_executable(2-TaggedMetrics 2-TaggedMetrics.cxx)
target_link_libraries(2-TaggedMetrics DataCollector)

add_executable(3-UserDefinedTimestamp 3-UserDefinedTimestamp.cxx)
target_link_libraries(3-UserDefinedTimestamp DataCollector)

add_executable(4-RateDerivedMetric 4-RateDerivedMetric.cxx)
target_link_libraries(4-RateDerivedMetric DataCollector)

add_executable(5-Benchmark 5-Benchmark.cxx)
target_link_libraries(5-Benchmark DataCollector)

add_executable(6-DedicatedInstance 6-DedicatedInstance.cxx)
target_link_libraries(6-DedicatedInstance DataCollector)
