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

#include "Monitoring/Metric.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// Types of ProcessMonitor parameters (eg. output of ps command)
enum class ProcessMonitorType { INT, DOUBLE, STRING };

/// Monitors current process and/or other processes running at the same machien
class ProcessMonitor
{
  friend class Collector;

  public:
    /// Prepares externam software commands (ps)
    ProcessMonitor();
	
    /// Default destructor
    ~ProcessMonitor() = default;

    /// Generates vector of tuples; singile tuple contain a meric for one PIDs
    /// \return	vactor of tuples; 3 values (type, name, value)
    std::vector<Metric> getPidsDetails();

  private:
    /// PIDs that are monitored
    unsigned int mPid;

    /// options to be passed to PS
    std::string mPsCommand;

    /// mutex to lock vector of PIDs
    std::mutex mVectorPidLock;

    /// List of PS params with their types
    const std::vector<std::pair<std::string, MetricType>> mPsParams { 
      {"etime", MetricType::STRING}, {"time", MetricType::STRING},
      {"pcpu", MetricType::DOUBLE}, {"pmem", MetricType::DOUBLE} };

    /// Executes terminal command
    std::string exec(const char* cmd);

    /// Vector of PID's parameters and values
    std::vector<std::string> getPidStatus(int pid);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
