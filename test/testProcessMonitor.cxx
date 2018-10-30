#include "Monitoring/ProcessMonitor.h"
#include "Monitoring/MonitoringFactory.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessMonitor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(createProcessMonitor)
{
  o2::monitoring::ProcessMonitor processMonitor;
  processMonitor.getCpuAndContexts();
  #ifdef _OS_LINUX
  processMonitor.getMemoryUsage();
  #endif
}

BOOST_AUTO_TEST_CASE(monitorProcess)
{
  auto monitoring = o2::monitoring::MonitoringFactory::Get("stdout://");
  monitoring->enableProcessMonitoring(1);
  std::this_thread::sleep_for (std::chrono::milliseconds(2100));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
