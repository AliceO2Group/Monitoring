#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE TestMonitoringFactory
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "../include/Monitoring/MonitoringFactory.h"

namespace o2 {
namespace monitoring {
namespace Test {

using Monitoring = o2::monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(verbosity)
{
   std::string ilUrl = "infologger://";
   auto il = Monitoring::GetBackend(ilUrl);
   BOOST_CHECK_EQUAL(static_cast<std::underlying_type<backend::Verbosity>::type>(il->getVerbosity()), 1); 

   std::string influxUrl = "influxdb-udp://127.0.0.1:1234";
   auto influx = Monitoring::GetBackend(influxUrl);
   BOOST_CHECK_EQUAL(static_cast<std::underlying_type<backend::Verbosity>::type>(influx->getVerbosity()), 0); 

   std::string influxDebugUrl = "influxdb-udp://127.0.0.1:1234/debug";
   auto influxDebug = Monitoring::GetBackend(influxDebugUrl);
   BOOST_CHECK_EQUAL(static_cast<std::underlying_type<backend::Verbosity>::type>(influxDebug->getVerbosity()), 1); 

   std::string ilProdUrl = "infologger:///prod";
   auto ilProd = Monitoring::GetBackend(ilProdUrl);
   BOOST_CHECK_EQUAL(static_cast<std::underlying_type<backend::Verbosity>::type>(ilProd->getVerbosity()), 0); 

}

} // namespace Test
} // namespace monitoring
} // namespace o2
