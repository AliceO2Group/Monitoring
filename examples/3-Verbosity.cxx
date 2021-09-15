///
/// \file 3-Verbosity.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{
  // Configure monitoring
  // Pass string with list of URLs as parameter
  // Set backend verbosities after as URL path
  auto monitoring = MonitoringFactory::Get("stdout:///debug,influxdb-stdout:///prod");

  monitoring->send({1, "myMetricInfo"}); // By default metric verbosity is set to Info
  monitoring->send({1, "myMetricInfo"});

  /// Verbosity is supported by buffering...
  monitoring->enableBuffering();
  monitoring->send({10, "myMetricDebug", Verbosity::Debug});
  monitoring->send({10.10, "myMetricProd", Verbosity::Prod});

  /// ... and derived metric modes
  monitoring->send({10, "myMetricDebug", Verbosity::Debug}, DerivedMetricMode::INCREMENT);
  monitoring->send({10.10, "myMetricProd", Verbosity::Prod}, DerivedMetricMode::INCREMENT);

  monitoring->send({10, "myMetricDebug", Verbosity::Debug}, DerivedMetricMode::INCREMENT);
  monitoring->send({10.10, "myMetricProd", Verbosity::Prod}, DerivedMetricMode::INCREMENT);
}
