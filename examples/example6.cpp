#include <iostream>
#include "Monitoring/ProcessMonitor.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {

  ConfigFile mConfigFile;
  mConfigFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini");

  // create monitoring object and confuguration as parameter to constructor
  std::shared_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector(mConfigFile));
	
  // create monitoring object and confuguration as parameter to constructor
  std::unique_ptr<Monitoring::Core::ProcessMonitor> monitor(new Monitoring::Core::ProcessMonitor(collector, 5));
  monitor->startMonitor();

  for (;;) {
    collector->send(10, "mainThreadMetric");
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}	
