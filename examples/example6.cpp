#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {

  ConfigFile configFile;
  configFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini");

  // create monitoring object and confuguration as parameter to constructor
  std::shared_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector(configFile));

  // add additional PID to be monitored
  collector->addMonitoredPid(1);
	
  for (;;) {
    collector->send(10, "mainThreadMetric");
    // manual monitor update
    collector->monitorUpdate();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}	
