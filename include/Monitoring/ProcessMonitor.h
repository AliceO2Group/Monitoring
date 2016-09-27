///
/// \file ProcessMonitor.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
#define ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

#include <atomic>
#include <boost/algorithm/string.hpp>
#include <mutex>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// Types of parameters
enum class ProcessMonitorType { INT, DOUBLE, STRING };

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
  std::vector<std::tuple<ProcessMonitorType, std::string, std::string>> getPidsDetails();

  /// Adds to list monitored PIDs
  /// \param pid
  void addPid(int pid);

private:
  /// Vector of PIDs that will be monitored
  std::vector<int> mPids;

  /// options to be passed to PS
  std::string mPsCommand;

  /// mutex to lock vector of PIDs
  std::mutex mVectorPidLock;

  /// List of PS params with their types
  const std::vector<std::pair<std::string, ProcessMonitorType>> psParams { 
    {"pid", ProcessMonitorType::INT},    {"etime", ProcessMonitorType::STRING}, {"time", ProcessMonitorType::STRING},
    {"pcpu", ProcessMonitorType::DOUBLE}, {"pmem", ProcessMonitorType::DOUBLE},   {"rsz", ProcessMonitorType::INT},
    {"vsz", ProcessMonitorType::INT},    {"comm", ProcessMonitorType::STRING} };

  /// Executes terminal command
  std::string exec(const char* cmd);

  /// Vector of PID's parameters and values
  std::vector<std::string> getPidStatus(int pid);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
