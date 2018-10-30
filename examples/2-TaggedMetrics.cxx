///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = o2::monitoring::MonitoringFactory;
using o2::monitoring::Metric;

int main() {

  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = Monitoring::Get("stdout://");

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then vector of key-value tags
  monitoring->send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
}
