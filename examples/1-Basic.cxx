///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ExampleBoilerplate.cxx"
#include "Monitoring/MonitoringFactory.h"
#include "Monitoring/Collector.h"

//using Backends = AliceO2::Monitoring::Backends;
using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main(int argc, char *argv[]) {
/*
  // configure monitoring (once per process), pass configuration path as parameter
  Monitoring::Configure("file://" + GetConfigFromCmdLine(argc, argv));

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  Monitoring::Get().send(10, "myMetricInt");
  Monitoring::Get().send(10.10, "myMetricFloat");
  
  // 2. by creating and moving metric object
  Monitoring::Get().send({10, "myMetricInt"});
  Monitoring::Get().send({10.10, "myMetricFloat"});
*/
  auto collector = std::make_unique<AliceO2::Monitoring::Collector>();
  collector->addBackend<AliceO2::Monitoring::Backends::InfluxDB>("udp://pcaldadam:365");
}	
