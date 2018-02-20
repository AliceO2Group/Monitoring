#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE Test Monitoring Colletor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "../include/Monitoring/MonitoringFactory.h"

namespace AliceO2 {
namespace Monitoring {
namespace Test {

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(createMonitoring)
{
  auto monitoring = Monitoring::Get("infologger://");

  int intMetric = 10; 
  std::string stringMetric("monitoringString");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

  monitoring->send(intMetric, "myCrazyMetricI");
  monitoring->send(stringMetric, "myCrazyMetricS");
  monitoring->send(doubleMetric, "myCrazyMetricD");
}

BOOST_AUTO_TEST_CASE(testIncrement)
{
  int value = 5;
  double dValue = 5.1;
  auto monitoring = Monitoring::Get("infologger://");
  monitoring->increment(value, "test");// value);
  monitoring->increment(value, "test");// value*2);
  monitoring->increment(dValue, "test2");// dValue);
  monitoring->increment(dValue, "test2");// dValue*2);
  monitoring->increment(value, "test");// value*3);
}

BOOST_AUTO_TEST_CASE(testSymbols)
{
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_APPMON ), "ApMon Backend disabled");
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_INFLUX ), "InfluxDB Backend disabled");
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_FLUME ), "Flume backend disabled");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_LINUX ), "Linux OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_WINDOWS ), "Windows OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_MAC ), "Mac OS detected");
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
