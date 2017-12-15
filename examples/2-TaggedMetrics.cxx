///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ExampleBoilerplate.cxx"
#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;
using AliceO2::Monitoring::Metric;

int main(int argc, char *argv[]) {

  // configure monitoring (once per process), pass configuration path as parameter
  Monitoring::Configure("file://" + GetConfigFromCmdLine(argc, argv));

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then vector of key-value tags
  Monitoring::Get().send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
}
