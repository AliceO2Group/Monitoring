#include "../src/Backends/Flume.h"

#define BOOST_TEST_MODULE testFlume
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(checkJsonEncoding)
{
  AliceO2::Monitoring::Backends::Flume flumeBackend("localhost", 1000);
  AliceO2::Monitoring::Metric metric{10, "myCrazyMetric"};
  flumeBackend.send(metric);
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
