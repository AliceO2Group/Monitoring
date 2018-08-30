Set(CTEST_PROJECT_NAME Monitoring)
Set(CTEST_BUILD_NAME Monitoring_Travis_CI)
Set(CTEST_SITE Travis)
Set(CTEST_SOURCE_DIRECTORY ".")
Set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
Find_Program(CTEST_GIT_COMMAND NAMES git)
Set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

Set(BUILD_COMMAND "make")
Set(CTEST_BUILD_COMMAND "${BUILD_COMMAND} -j")

set(CTEST_USE_LAUNCHERS 1)

Find_Program(GCOV_PATH gcov)
If(GCOV_PATH)
  Message("Found GCOV: ${GCOV_PATH}")
  Set(CTEST_COVERAGE_COMMAND ${GCOV_PATH})
  set(ENV{CXXFLAGS} "--coverage")
EndIf()

Set(CTEST_CONFIGURE_COMMAND " \"${CMAKE_EXECUTABLE_NAME}\" \"-G${CTEST_CMAKE_GENERATOR}\" \"${CTEST_SOURCE_DIRECTORY}\" ")

ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
If(GCOV_PATH)
  ctest_coverage(BUILD "${CTEST_BINARY_DIRECTORY}" LABELS coverage)
EndIf()

Execute_Process(COMMAND curl https://codecov.io/bash -o codecov_uploader.sh
                WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}
                TIMEOUT 60)
Execute_Process(COMMAND bash ./codecov_uploader.sh -X gcov
                WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}
                TIMEOUT 60)

Ctest_Submit()
