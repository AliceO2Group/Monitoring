#include "../include/Monitoring/MonitoringFactory.h"
#include <chrono>
#include <thread>
#include <vector>

#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE testCollector
#include <boost/test/included/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(derivedAverage)
{
  boost::filesystem::path relativePath = boost::filesystem::system_complete("../Monitoring/test/test.ini");
  Monitoring::Configure("file://" + relativePath.string());

  int intMetric = 10;
  std::string stringMetric("monitoring string");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

  Monitoring::Get().send(intMetric, "myCrazyMetricI");
  Monitoring::Get().send(stringMetric, "myCrazyMetricS");
  Monitoring::Get().send(doubleMetric, "myCrazyMetricD");
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
