#include "../include/Monitoring/Collector.h"
#include <chrono>
#include <thread>
#include <vector>

#define BOOST_TEST_MODULE testCollector
#include <boost/test/included/unit_test.hpp>

#include <Configuration/ConfigurationFactory.h>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(derivedAverage)
{
  std::unique_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector("file://../../Monitoring/test/test.ini"));
  
  int intMetric = 10;
  std::string stringMetric("monitoring string");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
  uint32_t uint32Metric = 555;

  collector->send(intMetric, "myCrazyMetricI");
  collector->send(stringMetric, "myCrazyMetricS");
  collector->send(doubleMetric, "myCrazyMetricD");
  collector->send(uint32Metric, "myCrazyMetricU");
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
