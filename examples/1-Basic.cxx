///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("stdout://");
  
  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric by creating and moving Metric object
  monitoring->send({10, "myMetricInt"});
  monitoring->send({10.10, "myMetricFloat"});
}	
