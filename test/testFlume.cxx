#include "../src/Backends/Flume.h"
#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(checkJsonEncoding)
{
  std::string url = "flume://localhost:1000";
  AliceO2::Monitoring::Backends::Flume flumeBackend(http::ParseHttpUrl(url));
  AliceO2::Monitoring::Metric metric{10, "myCrazyMetric"};
  flumeBackend.send(metric);
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
