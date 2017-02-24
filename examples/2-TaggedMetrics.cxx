///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;
using AliceO2::Monitoring::Metric;

int main() {
  try {
    // configure monitoring (once per process), pass configuration path as parameter
    Monitoring::Configure("file://../Monitoring/examples/SampleConfig.ini");
  } catch (std::string &e) {
    std::cout << "Run you examples from 'build' (dev) or 'bin' (install) direcotry\n";
    std::cout << e << std::endl;
  }

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then vector of key-value tags
  //
  // 1. by copying value and name and moving tags
  Monitoring::Get().sendTagged(10, "myMetric", {{"tag1", "value1"}, {"tag2", "value2"}});

  // 2. by moving value, name and tags
  Monitoring::Get().send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
}
