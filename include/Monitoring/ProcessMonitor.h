// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file ProcessMonitor.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
#define ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

#include <atomic>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sys/resource.h>

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
  enum {
    MEMORY_USAGE_PERCENTAGE = 0,
    VIRTUAL_MEMORY_SIZE,
    RESIDENT_SET_SIZE,
    CPU_USED_PERCENTAGE,
    INVOLUNTARY_CONTEXT_SWITCHING,
    CPU_USED_ABSOLUTE,
    AVG_RESIDENT_SET_SIZE,
    AVG_VIRTUAL_MEMORY_SIZE,
    AVG_CPU_USED_PERCENTAGE,
    ACCUMULATED_CPU_TIME,
    AVAILABLE_METRICS_SIZE
  };

  static std::vector<std::string> getAvailableMetricsNames();
  std::vector<Metric> getPerformanceMetrics();

 public:
  /// Prepares externam software commands (ps)
  ProcessMonitor();

  /// Default destructor
  ~ProcessMonitor() = default;

  void init();

 private:
  double splitStatusLineAndRetriveValue(const std::string& line) const;

  /// Retrievs total memory size from /proc/meminfo
  void setTotalMemory();

 private:
  static constexpr const char* metricsNames[] = {"memoryUsagePercentage", "virtualMemorySize", "residentSetSize",
                                                 "cpuUsedPercentage", "involuntaryContextSwitches", "cpuUsedAbsolute",
                                                 "averageResidentSetSize", "averageVirtualMemorySize", "averageCpuUsedPercentage",
                                                 "cpuTimeConsumedByProcess"};

  static constexpr unsigned int VM_SIZE_INDEX = 18;
  static constexpr unsigned int VM_RSS_INDEX = 22;

  /// PIDs that are monitored
  unsigned int mPid;

  /// Total memory size
  unsigned int mTotalMemory;

  /// 'getrusage' values from last execution
  struct rusage mPreviousGetrUsage;

  ///each measurement will be saved to compute average/accumulation usage
  std::vector<double> mVmSizeMeasurements;
  std::vector<double> mVmRssMeasurements;
  std::vector<uint64_t> mCpuMicroSeconds;
  std::vector<double> mCpuPerctange;

  /// Timestamp when process monitoring was executed last time
  std::chrono::high_resolution_clock::time_point mTimeLastRun;

  /// Retrieves virtual memory and resident set size usage
  std::vector<Metric> getMemoryUsage();

  /// Retrieves CPU usage (%) and number of context switches during the interval
  std::vector<Metric> getCpuAndContexts();

  std::vector<Metric> makeLastMeasurementAndGetMetrics();
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
