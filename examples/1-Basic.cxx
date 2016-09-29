///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
  
  // create monitoring object, pass confuguration path as parameter
  std::unique_ptr<Monitoring::Core::Collector> collector(
    new Monitoring::Core::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  // now send an application specific metric
  // 10 is the value
  // myCrazyMetric is the name of the metric
  collector->send(10, "myCrazyMetric");

}	
