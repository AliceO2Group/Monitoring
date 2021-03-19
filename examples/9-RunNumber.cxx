///
/// \file 9-RunNumber.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{

  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");

  monitoring->send(Metric{10, "myMetric"});
  monitoring->setRunNumber(1);
  monitoring->send(Metric{10, "myMetric"});
  monitoring->setRunNumber(2);
  monitoring->send(Metric{10, "myMetric"});
}
