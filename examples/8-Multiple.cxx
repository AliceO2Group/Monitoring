///
/// \file 8-Multiple.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = o2::monitoring::MonitoringFactory;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = Monitoring::Get("stdout://");

  monitoring->sendGrouped("measurementName", {{20, "myMetricIntMultiple"}, {20.30, "myMetricFloatMultple"}});
  monitoring->send({{201, "myMetricIntMultiple"}, {2.34, "myMetricFloatMultple"}});
}
