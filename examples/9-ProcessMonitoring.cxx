///
/// \file 9-ProcessMonitoring.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "Monitoring/ProcessMonitor.h"

using namespace o2::monitoring;

int main()
{
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
  auto processMonitor = std::make_unique<ProcessMonitor>();
  processMonitor->init();
  for (int i = 0; i < 5; i++) {
    for (auto&& metric : processMonitor->getPerformanceMetrics())
      monitoring->send(std::move(metric));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  for (auto&& metric : processMonitor->makeLastMeasurementAndGetMetrics())
    monitoring->send(std::move(metric));
}
