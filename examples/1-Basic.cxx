///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  
  // configure monitoring (once per process), pass configuration path as parameter
  Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  Monitoring::Get().send(10, "myMetric");
  
  // 2. by creating and moving metric object
  Monitoring::Get().send({10, "myMetric"});
}	
