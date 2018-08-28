#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  boost::filesystem::path configPath = boost::filesystem::canonical(".");
  auto monitoring = MonitoringFactory::Get("apmon://" + configPath.string() + "/ApMon.conf");
  monitoring->send({10, "myCrazyMetric"});
}



} // namespace Test
} // namespace monitoring
} // namespace o2
