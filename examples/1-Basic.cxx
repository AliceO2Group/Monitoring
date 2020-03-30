///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric by creating and moving Metric object
  monitoring->send(Metric{"myMetricInt"}.addValue("value", 10));
  monitoring->send(Metric{"myMetricFloat"}.addValue("value", 10.10));
}
