///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Monitoring/Collector.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  // configure monitoring (only once per process), pass configuration path as parameter
  Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

  // derived metric :  rate
  Monitoring::Get().addDerivedMetric("myMetric", Monitoring::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  Monitoring::Get().send(10, "myMetric");
  Monitoring::Get().send(20, "myMetric");
  Monitoring::Get().send(30, "myMetric");
  Monitoring::Get().send(50, "myMetric");
}
