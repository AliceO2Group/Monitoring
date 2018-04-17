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

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// Monitors current process and/or other processes running at the same machien
class ProcessMonitor
{
  friend class Monitoring;

  public:
    /// Prepares externam software commands (ps)
    ProcessMonitor();
	
    /// Default destructor
    ~ProcessMonitor() = default;

    /// Retrieves memory usage (%)
    Metric getMemoryUsage();

    /// Retrieves bytesReceived, bytesTransmitted per network interface
    std::vector<Metric> getNetworkUsage();

    /// Retrieves CPU usage (%) and number of context switches during the interval
    std::vector<Metric> ProcessMonitor::getCpuAndContexts()
  private:
    /// PIDs that are monitored
    unsigned int mPid;

    /// options to be passed to PS
    std::string mPsCommand;

    /// Executes terminal command
    std::string exec(const char* cmd);
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
