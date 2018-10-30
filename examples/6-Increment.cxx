///
/// \file 6-Increment.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = o2::monitoring::MonitoringFactory;
using DerivedMetricMode = o2::monitoring::DerivedMetricMode;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = Monitoring::Get("stdout://");

  // Increment values by ..
  monitoring->send({10, "myIncrementMetric"}, DerivedMetricMode::INCREMENT);
  monitoring->send({5, "myIncrementMetric"}, DerivedMetricMode::INCREMENT);
  monitoring->send({15, "myIncrementMetric"}, DerivedMetricMode::INCREMENT);
}	
