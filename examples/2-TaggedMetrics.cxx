///
/// \file 2-UserDefinedEntity.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring::Core;
using Monitoring::Metric;

int main() {
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  // now send an application specific metric
  // 10 is the value
  // myCrazyMetric is the name of the metri
  collector->send(Metric{10, "myCrazyMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
}
