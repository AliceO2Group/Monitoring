find_package(Boost COMPONENTS unit_test_framework program_options system filesystem REQUIRED)
find_package(Git QUIET)
find_package(APMON)
find_package(CURL)
find_package(Configuration REQUIRED)

set(extra_deps "")
if(APMON_FOUND)
    list(APPEND extra_deps ${APMON_LIBRARIES})
endif()
if(CURL_FOUND)
    list(APPEND extra_deps ${CURL_LIBRARIES})
endif()

o2_define_bucket(
        NAME
        o2_monitoring_bucket

        DEPENDENCIES
        Common
        InfoLogger
        ${Configuration_LIBRARIES}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${extra_deps}

        SYSTEMINCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS}
        ${Configuration_INCLUDE_DIRS}
)
