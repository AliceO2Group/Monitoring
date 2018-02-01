#include "../src/Backends/Zabbix.h"
#include "../src/Exceptions/MonitoringInternalException.h"
#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE testZabbix
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(crateZabbixInstance)
{
  try {
    std::string url = "zabbix://localhost:1000";
    AliceO2::Monitoring::Backends::Zabbix zabbixBackend(http::ParseHttpUrl(url));
  } catch (MonitoringInternalException &e) {
    BOOST_CHECK_EQUAL(std::string(e.what()), "Connection refused");
  }
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
