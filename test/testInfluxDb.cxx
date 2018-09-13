#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../src/Backends/InfluxDB.h"


namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  std::string url = "influxdb-udp://localhost:1000";
  auto parsed = http::ParseHttpUrl(url);
  o2::monitoring::backends::InfluxDB influxBackend(parsed.host, parsed.port);
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  influxBackend.send(metric);
}



} // namespace Test
} // namespace monitoring
} // namespace o2
