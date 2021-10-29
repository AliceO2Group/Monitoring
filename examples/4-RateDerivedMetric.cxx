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
  for (int i = 0; i < 41; i += 10) {
    monitoring->send(Metric{i, "myMetric"}.addTag(tags::Key::Subsystem, tags::Value::Readout), DerivedMetricMode::RATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // present "suppress" mode
  for (int i = 0; i < 10; i++) {
    monitoring->send(Metric{1, "slowMetric"}, DerivedMetricMode::SUPPRESS); // only 1 should be displayed
  }
  DerivedMetrics::mSuppressTimeout = std::chrono::seconds(1); // change timeout to 1 s
  monitoring->send(Metric{2, "slowMetric"}, DerivedMetricMode::SUPPRESS); // value changes
  std::this_thread::sleep_for(std::chrono::seconds(1));
  monitoring->send(Metric{2, "slowMetric"}, DerivedMetricMode::SUPPRESS); // timeout reached !
}
