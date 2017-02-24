///
/// \file 4-RateDerivedMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  try {
    // configure monitoring (once per process), pass configuration path as parameter
    Monitoring::Configure("file://../Monitoring/examples/SampleConfig.ini");
  } catch (std::string &e) {
    std::cout << "Run you examples from 'build' (dev) or 'bin' (install) direcotry\n";
    std::cout << e << std::endl;
  }

  // configure monitoring (only once per process), pass configuration path as parameter
  Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

  // derived metric :  rate
  Monitoring::Get().addDerivedMetric("myMetric", AliceO2::Monitoring::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  Monitoring::Get().send(10, "myMetric");
  Monitoring::Get().send(20, "myMetric");
  Monitoring::Get().send(30, "myMetric");
  Monitoring::Get().send(50, "myMetric");
}
