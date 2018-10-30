///
/// \file 9-Timer.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <chrono>

using Monitoring = o2::monitoring::MonitoringFactory;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = Monitoring::Get("stdout://");
  
  // Time the execution of the code below
  monitoring->startTimer("measureSleep");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  monitoring->stopAndSendTimer("measureSleep");
}	
