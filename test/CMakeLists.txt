# @author  Barthélémy von Haller

# Compile C++ tests (here only one, but it will grab whatever cpp is in the test directory)
file(GLOB_RECURSE TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cxx)

# Iterate over all tests found. For each, declare an executable and add it
# to the tests list.
set(ALL_TESTS)
find_package(Boost COMPONENTS system filesystem REQUIRED)
foreach(FILE ${TEST_FILES})
    string(REGEX REPLACE "[./]" "_" NAME ${FILE}) # create exe name
    add_executable(${NAME} ${FILE})
    target_link_libraries(${NAME} DataCollector ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    add_test(NAME ${NAME} COMMAND ${NAME})                              # this is how to add tests to CMake
endforeach(FILE ${TEST_FILES})

include(CTest)
