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
  for (int i = 0; i < 101; i+=10) {
    Monitoring::Get().send(i, "myMetric");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}
