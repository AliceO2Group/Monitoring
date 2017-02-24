///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ExampleBoilerplate.cxx"
#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main(int argc, char *argv[]) {
  // configure monitoring (once per process), pass configuration path as parameter
  Monitoring::Configure("file://" + GetConfigFromCmdLine(argc, argv));

  // derived metric :  rate
  Monitoring::Get().addDerivedMetric("myMetric", AliceO2::Monitoring::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  Monitoring::Get().send(10, "myMetric");
  Monitoring::Get().send(20, "myMetric");
  Monitoring::Get().send(30, "myMetric");
  Monitoring::Get().send(50, "myMetric");
}
