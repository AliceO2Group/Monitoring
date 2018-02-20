#include "Monitoring/ProcessMonitor.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessMonitor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(createProcessMonitor)
{
  o2::monitoring::ProcessMonitor processMonitor;
  processMonitor.getNetworkUsage();
  processMonitor.getPidStatus();
}

} // namespace Test
} // namespace monitoring
} // namespace o2
