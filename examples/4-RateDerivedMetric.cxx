///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto collector = Monitoring::Get("infologger://");

  // derived metric :  rate
  collector->addDerivedMetric("myMetric", AliceO2::Monitoring::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  for (int i = 0; i < 101; i += 10) {
    collector->send(i, "myMetric");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}
