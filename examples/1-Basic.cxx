///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
  // parse configuration file
  ConfigFile configFile;
  configFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");	

  // create monitoring object and confuguration as parameter to constructor
  std::unique_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector(configFile));

  // now send an application specific metric
  // 10 is the value
  // myCrazyMetric is the name of the metric
  collector->send(10, "myCrazyMetric");

}	
