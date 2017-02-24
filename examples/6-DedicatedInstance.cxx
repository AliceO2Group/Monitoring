//
/// \file 6-DedicatedInstance.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ExampleBoilerplate.cxx"
#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main(int argc, char *argv[]) {
  
  // create dedicated monitoring instance, pass confuguration path as parameter
  auto collector = Monitoring::Create("file://" + GetConfigFromCmdLine(argc, argv));

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  collector->send(10, "myMetric");
  
  // 2. by creating and moving metric object
  collector->send({10, "myMetric"});
}	
