///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = o2::monitoring::MonitoringFactory;
using DerivedMetricMode =  o2::monitoring::DerivedMetricMode;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = Monitoring::Get("infologger://");

  // now send at least two metrics to see the result
  for (int i = 0; i < 101; i += 10) {
    monitoring->send({i, "myMetric"}, DerivedMetricMode::RATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}
