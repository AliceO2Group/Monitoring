#include "../include/Monitoring/TemplatedMetric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE testTemplatedMetric
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Core {

BOOST_AUTO_TEST_CASE(retrieveInt)
{
	int value = 10;
	std::string name("metric name");
	std::string entity("metric entity");
	std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

	AliceO2::Monitoring::Core::TemplatedMetric<int> metricInstance(value,  name, entity, timestamp );

	BOOST_CHECK_EQUAL(metricInstance.getValue(), 10);

}

BOOST_AUTO_TEST_CASE(retrieveString)
{
        std::string value = "test";
        std::string name("metricName");
        std::string entity("metricEntity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::TemplatedMetric<std::string> metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(metricInstance.getValue(), "test");

}

BOOST_AUTO_TEST_CASE(retrieveDouble)
{
        double value = 14.1234567;
        std::string name("metricName");
        std::string entity("metricEntity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::TemplatedMetric<double> metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(metricInstance.getValue(), 14.1234567);

}

BOOST_AUTO_TEST_CASE(retrieveUint_32t)
{
        uint32_t value = 14;
        std::string name("metricName");
        std::string entity("metricEntity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

        AliceO2::Monitoring::Core::TemplatedMetric<uint32_t> metricInstance(value,  name, entity, timestamp );

        BOOST_CHECK_EQUAL(metricInstance.getValue(), 14);

}

BOOST_AUTO_TEST_CASE(subtractInt)
{
		
	int value = 14;
	int value2 = 22;

        std::string name("metricName");
        std::string entity("metricEntity");
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::chrono::time_point<std::chrono::system_clock> timestamp2 = std::chrono::system_clock::now();
	
	Metric *big = new TemplatedMetric<int>(value2, name, entity, timestamp2);
	Metric *small = new TemplatedMetric<int>(value, name, entity, timestamp);
	Metric *subtract = big->subtract(small);
	double result = static_cast<TemplatedMetric<double>*>(subtract)->getValue();
	
	BOOST_TEST(result < 8.5);
	BOOST_TEST(result > 7.5);

}




} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
