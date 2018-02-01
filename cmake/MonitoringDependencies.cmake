find_package(Boost COMPONENTS unit_test_framework program_options system filesystem REQUIRED)
find_package(Git QUIET)
find_package(APMON)
find_package(CURL)

set(extra_deps "")
set(extra_deps_include "")
if (APMON_FOUND)
    list(APPEND extra_deps ${APMON_LIBRARIES})
    list(APPEND extra_deps_include ${APMON_INCLUDE_DIRS})
endif ()
if (CURL_FOUND)
    list(APPEND extra_deps ${CURL_LIBRARIES})
    list(APPEND extra_deps_include ${CURL_INCLUDE_DIRS})
endif ()

o2_define_bucket(
    NAME
    o2_monitoring_bucket

    DEPENDENCIES
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${extra_deps}

    SYSTEMINCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
    ${extra_deps_include}
)
