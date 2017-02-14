///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;
using Monitoring::Metric;

int main() {
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then vector of key-value tags
  //
  // 1. by copying value and name and moving tags
  collector->sendTagged(10, "myMetric", {{"tag1", "value1"}, {"tag2", "value2"}});

  // 2. by moving value, name and tags
  collector->send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
}
