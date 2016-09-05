#ifndef ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
#define ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <boost/algorithm/string.hpp>

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

/// \brief Allows self monitoring or to monitor processes running at the same machine
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>

class ProcessMonitor {

friend class Collector;

public:
  /// Enables self monitoring
  /// \param collector	shared pointer to collector, it executes only thread-safe methods of Collector
  /// \param interval 	time interval between updates
  ProcessMonitor();
	
  /// Default destructor
  ~ProcessMonitor() = default;

  /// Generates vector of parameters for all PIDs
  /// \return	vactor of tuples; 3 values (type, name, value)
  std::vector<std::tuple<int, std::string, std::string>> getPidsDetails();

  /// Adds to list monitored PIDs
  /// \param pid
  void addPid(int pid);
private:

  /// Vector of PIDs that will be monitored
  std::vector<int> mPids;

  /// options to be passed to PS
  std::string mOptions;

  /// mutex to lock vector of PIDs
  std::mutex mVectorPidLock;

  /// List of PS params with their types: 0 - int, 1 - double, 2 - string
  const std::vector<std::pair<std::string, int>> params { {"pid", 0 }, {"etime", 2}, {"time", 2}, {"pcpu", 1 }, {"pmem", 1}, {"rsz", 0}, {"vsz", 0}, {"comm", 2} };

  /// Executes terminal command
  std::string exec(const char* cmd);

  /// Vector of PID's parameters and values
  std::vector<std::string> getPIDStatus(int pid);

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
