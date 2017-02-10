///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring::Core;
using Monitoring::Metric;

int main() {
  
  // create monitoring object, pass confuguration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  
  // moving
  collector->send(10, "myMetric");
  collector->send({20, "myMetric"});
  collector->send(Metric{30, "myMetric"});

  // passing by reference
  Metric metric{10, "myMetricAgain"};
  collector->send(metric);
}	
