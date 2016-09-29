///
/// \file 5-MonitorAdditionalPid.cxx
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

  // add additional PID to be monitored
  collector->addMonitoredPid(1);
	
  for (;;) {
    collector->send(10, "mainThreadMetric");
    // manual monitor update
    collector->sendProcessMonitorValues();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}	
