///
/// \file 9-ProcessMonitoring.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
  for (int i = 0; i < 5; i++) {
    monitoring->pushProcessMonitoringMetrics();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
