#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "../src/Backends/ApMonBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  boost::filesystem::path configPath = boost::filesystem::canonical("..");
  std::string url = "apmon://" + configPath.string() + "/test/ApMon.conf";
  auto parsed = http::ParseHttpUrl(url);
  std::cout << parsed.path << std::endl;
  AliceO2::Monitoring::Backends::ApMonBackend apMonBackend(parsed.path);
  AliceO2::Monitoring::Metric metric{10, "myCrazyMetric"};
  apMonBackend.send(metric);
}



} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
