///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Core::Collector> collector(
    new Monitoring::Core::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );  

  // derived metric :  rate
  collector->addDerivedMetric("myCrazyMetric1", Monitoring::Core::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  collector->send(10, "myCrazyMetric1");
  collector->send(20, "myCrazyMetric1");
  collector->send(30, "myCrazyMetric1");
  collector->send(50, "myCrazyMetric1");
}
