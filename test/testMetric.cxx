#include "../include/Monitoring/Metric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE testMetric
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Core {

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
        int value = 10;
        std::string name("metric name");
        std::string entity("metric entity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::Metric metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(metricInstance.getName(), "metric name");
        BOOST_CHECK_EQUAL(metricInstance.getEntity(), "metric entity");
}


BOOST_AUTO_TEST_CASE(retrieveInt)
{
	int value = 10;
	std::string name("metric name");
	std::string entity("metric entity");
	std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

	AliceO2::Monitoring::Core::Metric metricInstance(value,  name, entity, timestamp );

	BOOST_CHECK_EQUAL(boost::get<int>(metricInstance.getValue()), 10);
	BOOST_CHECK_EQUAL(metricInstance.getType(), 0);
}

BOOST_AUTO_TEST_CASE(retrieveDouble)
{
        double value = 1.11;
        std::string name("metric name");
        std::string entity("metric entity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::Metric metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(boost::get<double>(metricInstance.getValue()), 1.11);
	BOOST_CHECK_EQUAL(metricInstance.getType(), 2);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
        std::string value = "testString";
        std::string name("metric name");
        std::string entity("metric entity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::Metric metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(boost::get<std::string>(metricInstance.getValue()), "testString");
        BOOST_CHECK_EQUAL(metricInstance.getType(), 1);
}

BOOST_AUTO_TEST_CASE(retrieveUint32_t)
{
        uint32_t value = 55;
        std::string name("metric name");
        std::string entity("metric entity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::Metric metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(boost::get<uint32_t>(metricInstance.getValue()), 55);
        BOOST_CHECK_EQUAL(metricInstance.getType(), 3);
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
