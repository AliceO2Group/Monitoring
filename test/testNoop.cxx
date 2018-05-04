#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../src/Backends/Noop.h"

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(noopSendMetric)
{
  o2::monitoring::backends::Noop noopBackend{};
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  noopBackend.send(metric);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
