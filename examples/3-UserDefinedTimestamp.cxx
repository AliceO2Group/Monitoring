///
/// \file 3-UserDefinedTimestamp.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include <chrono>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;
using Monitoring::Metric;

int main() {
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  // current timestamp
  std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
	
  // now send an application specific metric
  // 10 is the value timestamp
  // myCrazyMetric is the name of the metric
  //
  // 1. by copying all parameters
  collector->sendTimed(10, "myCrazyMetric", timestamp);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  collector->sendTimed(40, "myCrazyMetric", timestamp);
  
  // 2. by moving
  collector->send(Metric{10, "myCrazyMetric"}.setTimestamp(timestamp));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  collector->send(Metric{40, "myCrazyMetric"}.setTimestamp(timestamp));
}
