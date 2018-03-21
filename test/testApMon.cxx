#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "../src/Backends/ApMonBackend.h"

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  boost::filesystem::path configPath = boost::filesystem::canonical(".");
  std::string url = "apmon://" + configPath.string() + "/ApMon.conf";
  auto parsed = http::ParseHttpUrl(url);
  std::cout << parsed.path << std::endl;
  o2::monitoring::backends::ApMonBackend apMonBackend(parsed.path);
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  apMonBackend.send(metric);
}



} // namespace Test
} // namespace monitoring
} // namespace o2
