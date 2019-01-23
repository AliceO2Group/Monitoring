///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("stdout:///info");
  
  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric by creating and moving Metric object
  monitoring->send({10, "myMetricInt", Verbosity::DEBUG}, DerivedMetricMode::INCREMENT);
  monitoring->send({10.10, "myMetricFloat", Verbosity::PROD}, DerivedMetricMode::INCREMENT);

  monitoring->sendGrouped("measurementName", {{20, "myMetricIntMultiple"}, {20.30, "myMetricFloatMultple"}}, Verbosity::DEBUG);
  monitoring->sendGrouped("measurementName", {{20, "myMetricIntMultiple"}, {20.30, "myMetricFloatMultple"}}, Verbosity::PROD);

  monitoring->send({10, "myMetricInt", Verbosity::DEBUG}, DerivedMetricMode::INCREMENT);
  monitoring->send({10.10, "myMetricFloat", Verbosity::PROD}, DerivedMetricMode::INCREMENT);

  monitoring->enableBuffering();
  monitoring->send({10, "myMetricInt", Verbosity::DEBUG});
  monitoring->send({10.10, "myMetricFloat", Verbosity::PROD});
}	
