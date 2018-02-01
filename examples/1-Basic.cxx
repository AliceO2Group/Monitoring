///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "Monitoring/Collector.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto collector = Monitoring::Get("infologger://");
  
  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  collector->send(10, "myMetricInt");
  collector->send(10.10, "myMetricFloat");
  
  // 2. by creating and moving metric object
  collector->send({10, "myMetricInt"});
  collector->send({10.10, "myMetricFloat"});
}	
