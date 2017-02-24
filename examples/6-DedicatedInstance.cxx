//
/// \file 6-DedicatedInstance.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  // create dedicated monitoring instance, pass confuguration path as parameter
  auto collector = Monitoring::Create("file://../Monitoring/examples/SampleConfig.ini");

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  collector->send(10, "myMetric");
  
  // 2. by creating and moving metric object
  collector->send({10, "myMetric"});
}	
