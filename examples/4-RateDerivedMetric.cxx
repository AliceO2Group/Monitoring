///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("stdout://");

  // now send at least two metrics to see the result
  for (int i = 0; i < 101; i += 10) {
    monitoring->send(Metric{i, "myMetric"}.addTag(tags::Key::Subsystem, tags::Value::Readout), DerivedMetricMode::RATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
