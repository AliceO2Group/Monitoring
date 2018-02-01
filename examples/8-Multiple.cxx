///
/// \file 8-Multiple.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto collector = Monitoring::Get("infologger://");

  collector->send("measurementName", {{20, "myMetricIntMultiple"}, {20.30, "myMetricFloatMultple"}});
}
