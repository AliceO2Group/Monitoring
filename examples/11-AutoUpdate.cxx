///
/// \file 11-AutoUpdate.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main() {
  auto monitoring = MonitoringFactory::Get("infologger://");

  // Enable periodical value pushing (default every 1s)
  monitoring->enableAutoPush();

  // Get reference to metrics
  auto& qcMetric = monitoring->getAutoPushMetric("qcMetric"); 
  auto& qcMetric2 = monitoring->getAutoPushMetric("qcMetric2");
  std::this_thread::sleep_for (std::chrono::milliseconds(2000));

  // Update values
  qcMetric = 123;
  qcMetric2 = 321;
  std::this_thread::sleep_for (std::chrono::milliseconds(2000));
}	
