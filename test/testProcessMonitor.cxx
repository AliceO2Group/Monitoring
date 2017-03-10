#include "../include/Monitoring/ProcessMonitor.h"

#define BOOST_TEST_MODULE testProcessMonitor
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(createProcessMonitor)
{
  AliceO2::Monitoring::ProcessMonitor processMonitor;
  processMonitor.getNetworkUsage();
  processMonitor.getPidStatus();
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
