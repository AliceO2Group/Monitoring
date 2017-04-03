#include "../include/Monitoring/MonitoringFactory.h"
#include <chrono>
#include <thread>
#include <vector>

#define BOOST_TEST_MODULE testCollector
#include <boost/test/included/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(createCollector)
{
  Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

  int intMetric = 10; 
  std::string stringMetric("monitoringString");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

  Monitoring::Get().send(intMetric, "myCrazyMetricI");
  Monitoring::Get().send(stringMetric, "myCrazyMetricS");
  Monitoring::Get().send(doubleMetric, "myCrazyMetricD");
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
