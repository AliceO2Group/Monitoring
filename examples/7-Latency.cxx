///
/// \file 7-Latency.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "ExampleBoilerplate.cxx"
#include <boost/program_options.hpp>
#include <random>

using Monitoring = AliceO2::Monitoring::MonitoringFactory;
using namespace AliceO2::Monitoring;

int main(int argc, char *argv[]) {
  Monitoring::Configure("file://" + GetConfigFromCmdLine(argc, argv));
  auto timestamp = std::chrono::system_clock::now();
  
  Monitoring::Get().send(
    Metric{999, "latencyTestMetric"}.addTags(
      {{"libraryTimestamp", std::to_string(
        std::chrono::duration_cast <std::chrono::nanoseconds>(timestamp.time_since_epoch()).count()
      )}} 
    )
  );
}	
