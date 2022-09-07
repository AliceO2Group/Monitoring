// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file ProcessMonitor.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ProcessMonitor.h"
#include "MonLogger.h"
#include <unistd.h>
#include <chrono>
#include <sstream>
#include <cmath>
#include <fstream>
#include <utility>
#include <numeric>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

#ifdef O2_MONITORING_OS_CS8
static constexpr auto SMAPS_FILE = "/proc/self/smaps_rollup";
#else
static constexpr auto SMAPS_FILE = "/proc/self/smaps";
#endif

ProcessMonitor::ProcessMonitor()
{
  mPid = static_cast<unsigned int>(::getpid());
  mTimeLastRun = std::chrono::high_resolution_clock::now();
  getrusage(RUSAGE_SELF, &mPreviousGetrUsage);
#ifdef O2_MONITORING_OS_LINUX
  setTotalMemory();
#endif
  mEnabledMeasurements.fill(false);
}

void ProcessMonitor::init()
{
  mTimeLastRun = std::chrono::high_resolution_clock::now();
  getrusage(RUSAGE_SELF, &mPreviousGetrUsage);
}

void ProcessMonitor::enable(Monitor measurement)
{
  mEnabledMeasurements[static_cast<short>(measurement)] = true;
}

void ProcessMonitor::setTotalMemory()
{
  std::ifstream memInfo("/proc/meminfo");
  std::string totalString;
  std::getline(memInfo, totalString);
  mTotalMemory = splitStatusLineAndRetriveValue(totalString);
}

std::vector<Metric> ProcessMonitor::getMemoryUsage()
{
  std::vector<Metric> metrics;
  std::ifstream statusStream("/proc/self/status");
  std::string rssString;
  rssString.reserve(50);

  // Scan for VmSize
  for (unsigned i = 0; i < VM_SIZE_INDEX; ++i) {
    std::getline(statusStream, rssString);
  }
  auto vmSize = splitStatusLineAndRetriveValue(rssString);
  mVmSizeMeasurements.push_back(vmSize);

  metrics.emplace_back((vmSize * 100) / mTotalMemory, metricsNames[MEMORY_USAGE_PERCENTAGE]);
  metrics.emplace_back(vmSize, metricsNames[VIRTUAL_MEMORY_SIZE]);

  // Scan for VmRSS
  for (unsigned i = 0; i < VM_RSS_INDEX - VM_SIZE_INDEX; ++i) {
    std::getline(statusStream, rssString);
  }

  auto vmRSS = splitStatusLineAndRetriveValue(rssString);
  metrics.emplace_back(vmRSS, metricsNames[RESIDENT_SET_SIZE]);
  mVmRssMeasurements.push_back(vmRSS);

  return metrics;
}

std::vector<Metric> ProcessMonitor::getSmaps()
{
  std::ifstream statusStream(SMAPS_FILE);
  double pssTotal = 0;
  double cleanTotal = 0;
  double dirtyTotal = 0;
  std::string smapsString;

  while (std::getline(statusStream, smapsString)) {
    if (smapsString.rfind("Pss:", 0) == 0) {
      pssTotal += splitStatusLineAndRetriveValue(smapsString);
    }
    if (smapsString.rfind("Private_Clean:", 0) == 0) {
      cleanTotal += splitStatusLineAndRetriveValue(smapsString);
    }
    if (smapsString.rfind("Private_Dirty:", 0) == 0) {
      dirtyTotal += splitStatusLineAndRetriveValue(smapsString);
    }
  }
  return {{pssTotal, metricsNames[PSS]}, {cleanTotal, metricsNames[PRIVATE_CLEAN]}, {dirtyTotal, metricsNames[PRIVATE_DIRTY]}};
}

std::vector<Metric> ProcessMonitor::getCpuAndContexts()
{
  std::vector<Metric> metrics;
  struct rusage currentUsage;
  getrusage(RUSAGE_SELF, &currentUsage);
  auto timeNow = std::chrono::high_resolution_clock::now();
  double timePassed = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - mTimeLastRun).count();
  if (timePassed < 950) {
    MonLogger::Get(Severity::Warn) << "Do not invoke Process Monitor more frequent then every 1s" << MonLogger::End();
    metrics.emplace_back("processPerformance");
    return metrics;
  }

  uint64_t cpuUsedInMicroSeconds = currentUsage.ru_utime.tv_sec * 1000000.0 + currentUsage.ru_utime.tv_usec - (mPreviousGetrUsage.ru_utime.tv_sec * 1000000.0 + mPreviousGetrUsage.ru_utime.tv_usec) + currentUsage.ru_stime.tv_sec * 1000000.0 + currentUsage.ru_stime.tv_usec - (mPreviousGetrUsage.ru_stime.tv_sec * 1000000.0 + mPreviousGetrUsage.ru_stime.tv_usec);
  double fractionCpuUsed = cpuUsedInMicroSeconds / timePassed;

  double cpuUsedPerctange = std::round(fractionCpuUsed * 100.0 * 100.0) / 100.0;
  mCpuPerctange.push_back(cpuUsedPerctange);
  mCpuMicroSeconds.push_back(cpuUsedInMicroSeconds);

  metrics.emplace_back(Metric{cpuUsedPerctange, metricsNames[CPU_USED_PERCENTAGE]});
  metrics.emplace_back(Metric{
    static_cast<uint64_t>(currentUsage.ru_nivcsw - mPreviousGetrUsage.ru_nivcsw), metricsNames[INVOLUNTARY_CONTEXT_SWITCHES]});
  metrics.emplace_back(Metric{
    static_cast<uint64_t>(currentUsage.ru_nvcsw - mPreviousGetrUsage.ru_nvcsw), metricsNames[VOLUNTARY_CONTEXT_SWITCHES]});
  metrics.emplace_back(cpuUsedInMicroSeconds, metricsNames[CPU_USED_ABSOLUTE]);

  mTimeLastRun = timeNow;
  mPreviousGetrUsage = currentUsage;
  return metrics;
}

double ProcessMonitor::splitStatusLineAndRetriveValue(const std::string& line) const
{
  std::istringstream iss(line);
  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
  return tokens.size() < 2 ? -1.0 : std::stod(tokens.at(1));
}

std::vector<Metric> ProcessMonitor::getPerformanceMetrics()
{
  std::vector<Metric> metrics;
  metrics.reserve(12);
  if (mEnabledMeasurements.at(static_cast<short>(Monitor::Cpu))) {
    auto cpuMetrics = getCpuAndContexts();
    std::move(cpuMetrics.begin(), cpuMetrics.end(), std::back_inserter(metrics));
  }
#ifdef O2_MONITORING_OS_LINUX
  if (mEnabledMeasurements.at(static_cast<short>(Monitor::Mem))) {
    auto memoryMetrics = getMemoryUsage();
    std::move(memoryMetrics.begin(), memoryMetrics.end(), std::back_inserter(metrics));
  }
  if (mEnabledMeasurements.at(static_cast<short>(Monitor::Smaps))) {
    auto smapMetrics = getSmaps();
    std::move(smapMetrics.begin(), smapMetrics.end(), std::back_inserter(metrics));
  }
#endif
  return metrics;
}

std::vector<Metric> ProcessMonitor::makeLastMeasurementAndGetMetrics()
{
  std::vector<Metric> metrics;
  getCpuAndContexts();
#ifdef O2_MONITORING_OS_LINUX
  getMemoryUsage();

  auto avgVmRSS = std::accumulate(mVmRssMeasurements.begin(), mVmRssMeasurements.end(), 0.0) /
                  mVmRssMeasurements.size();

  metrics.emplace_back(avgVmRSS, metricsNames[AVG_RESIDENT_SET_SIZE]);

  auto avgVmSize = std::accumulate(mVmSizeMeasurements.begin(), mVmSizeMeasurements.end(), 0.0) /
                   mVmSizeMeasurements.size();
  metrics.emplace_back(avgVmSize, metricsNames[AVG_VIRTUAL_MEMORY_SIZE]);
#endif

  auto avgCpuUsage = std::accumulate(mCpuPerctange.begin(), mCpuPerctange.end(), 0.0) /
                     mCpuPerctange.size();
  uint64_t accumulationOfCpuTimeConsumption = std::accumulate(mCpuMicroSeconds.begin(),
                                                              mCpuMicroSeconds.end(), 0UL);

  metrics.emplace_back(avgCpuUsage, metricsNames[AVG_CPU_USED_PERCENTAGE]);
  metrics.emplace_back(accumulationOfCpuTimeConsumption, metricsNames[ACCUMULATED_CPU_TIME]);

  return metrics;
}

std::vector<std::string> ProcessMonitor::getAvailableMetricsNames()
{
  return {std::begin(metricsNames), std::end(metricsNames)};
}

} // namespace monitoring
} // namespace o2
