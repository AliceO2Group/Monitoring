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

  // send a metric using one of two equivalent methods
  // 10 is the value
  // myMetric is the name of the metric by creating and moving Metric object
  monitoring->send({10, "myMetricInt"}); // default name is "value"
  monitoring->send(Metric{"myMetricInt"}.addValue(10, "value"));

  // now send a metric with multiple values
  monitoring->send(Metric{10, "myMetricInt"}.addValue(10.10, "value_float"));
  monitoring->send(Metric{"myMetricInt"}.addValue(10, "value").addValue(10.10, "value_float"));
}
