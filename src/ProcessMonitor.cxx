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
#include <iterator>
#include <sstream>
#include <cmath>
#include <fstream>
#include <utility>
#include <numeric>
#ifdef O2_MONITORING_OS_LINUX
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <cstring>
#endif

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

static constexpr auto SMAPS_ROLLUP_FILE = "/proc/self/smaps_rollup";
static constexpr auto SMAPS_FILE = "/proc/self/smaps";

#ifdef O2_MONITORING_OS_LINUX
namespace
{
struct InstrReadFormat {
  uint64_t value;
  uint64_t timeEnabled;
  uint64_t timeRunning;
};
inline long perfEventOpen(struct perf_event_attr* attr, pid_t pid, int cpu,
                          int group, unsigned long flags)
{
  return syscall(__NR_perf_event_open, attr, pid, cpu, group, flags);
}
} // namespace
#endif

ProcessMonitor::ProcessMonitor()
{
  mPid = static_cast<unsigned int>(::getpid());
  mTimeLastRun = std::chrono::high_resolution_clock::now();
  getrusage(RUSAGE_SELF, &mPreviousGetrUsage);
  getrusage(RUSAGE_CHILDREN, &mPreviousGetrUsageChildren);
#ifdef O2_MONITORING_OS_LINUX
  setTotalMemory();
#endif
  mEnabledMeasurements.fill(false);
  openInstructionCounter();
}

void ProcessMonitor::openInstructionCounter()
{
#ifdef O2_MONITORING_OS_LINUX
  struct perf_event_attr attr;
  std::memset(&attr, 0, sizeof(attr));
  attr.size = sizeof(attr);
  attr.type = PERF_TYPE_HARDWARE;
  attr.config = PERF_COUNT_HW_INSTRUCTIONS;
  attr.disabled = 0;       // count from the moment it is opened
  attr.exclude_kernel = 1; // user-space only: the relevant signal, and works at perf_event_paranoid <= 2
  attr.exclude_hv = 1;
  attr.inherit = 1;        // also count threads spawned afterwards (validated: live + exited threads aggregate)
  attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
  // pid 0 = this process (and inherited threads), cpu -1 = any. Best effort: a high
  // perf_event_paranoid, container seccomp, or a missing PMU simply leaves it disabled.
  mInstructionsFd = static_cast<int>(perfEventOpen(&attr, 0, -1, -1, 0));
#endif
}

ProcessMonitor::~ProcessMonitor()
{
#ifdef O2_MONITORING_OS_LINUX
  if (mInstructionsFd >= 0) {
    ::close(mInstructionsFd);
  }
#endif
}

void ProcessMonitor::init()
{
  mTimeLastRun = std::chrono::high_resolution_clock::now();
  getrusage(RUSAGE_SELF, &mPreviousGetrUsage);
  getrusage(RUSAGE_CHILDREN, &mPreviousGetrUsageChildren);
  // The aggregates cover one monitoring period: monitoring stopped and started
  // again reports the new period, not both blended together.
  mCpuPerctange.clear();
  mCpuMicroSeconds.clear();
  mVmSizeMeasurements.clear();
  mVmRssMeasurements.clear();
}

void ProcessMonitor::enable(PmMeasurement measurement)
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
  static const char* const smapsFile =
    (::access(SMAPS_ROLLUP_FILE, R_OK) == 0) ? SMAPS_ROLLUP_FILE : SMAPS_FILE;
  std::ifstream statusStream(smapsFile);
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

std::vector<Metric> ProcessMonitor::getCpuAndContexts(bool force)
{
  std::vector<Metric> metrics;
  // RUSAGE_SELF does not see work done by reaped children (e.g. an external
  // event generator forked by o2-sim), so every counter below sums the two.
  struct rusage currentUsage;
  struct rusage currentUsageChildren;
  getrusage(RUSAGE_SELF, &currentUsage);
  getrusage(RUSAGE_CHILDREN, &currentUsageChildren);
  auto timeNow = std::chrono::high_resolution_clock::now();
  double timePassed = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - mTimeLastRun).count();
  if (timePassed < 950 && !force) {
    MonLogger::Get(Severity::Warn) << "Do not invoke Process Monitor more frequent then every 1s" << MonLogger::End();
    metrics.emplace_back("processPerformance");
    return metrics;
  }

  // CPU time (user + system) of one snapshot, in microseconds
  auto cpuMicros = [](const struct rusage& usage) {
    return (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000000.0 + usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
  };
  uint64_t cpuUsedInMicroSeconds = (cpuMicros(currentUsage) - cpuMicros(mPreviousGetrUsage)) +
                                   (cpuMicros(currentUsageChildren) - cpuMicros(mPreviousGetrUsageChildren));
  mCpuMicroSeconds.push_back(cpuUsedInMicroSeconds);

  // A child's CPU time appears all at once when it is reaped, so the delta of a
  // forced (final) measurement is not a rate over the interval: absolute time only.
  if (!force) {
    double fractionCpuUsed = cpuUsedInMicroSeconds / timePassed;
    double cpuUsedPerctange = std::round(fractionCpuUsed * 100.0 * 100.0) / 100.0;
    mCpuPerctange.push_back(cpuUsedPerctange);
    metrics.emplace_back(Metric{cpuUsedPerctange, metricsNames[CPU_USED_PERCENTAGE]});
  }
  uint64_t involuntaryContextSwitches = (currentUsage.ru_nivcsw - mPreviousGetrUsage.ru_nivcsw) +
                                        (currentUsageChildren.ru_nivcsw - mPreviousGetrUsageChildren.ru_nivcsw);
  uint64_t voluntaryContextSwitches = (currentUsage.ru_nvcsw - mPreviousGetrUsage.ru_nvcsw) +
                                      (currentUsageChildren.ru_nvcsw - mPreviousGetrUsageChildren.ru_nvcsw);
  metrics.emplace_back(Metric{involuntaryContextSwitches, metricsNames[INVOLUNTARY_CONTEXT_SWITCHES]});
  metrics.emplace_back(Metric{voluntaryContextSwitches, metricsNames[VOLUNTARY_CONTEXT_SWITCHES]});
  metrics.emplace_back(cpuUsedInMicroSeconds, metricsNames[CPU_USED_ABSOLUTE]);

#ifdef O2_MONITORING_OS_LINUX
  if (mInstructionsFd >= 0) {
    InstrReadFormat rf;
    if (::read(mInstructionsFd, &rf, sizeof(rf)) == static_cast<ssize_t>(sizeof(rf))) {
      // Correct for PMU multiplexing: when the counter is not always scheduled,
      // timeEnabled > timeRunning, so scale the raw value back up to a full-time estimate.
      double scale = rf.timeRunning ? static_cast<double>(rf.timeEnabled) / rf.timeRunning : 1.0;
      uint64_t total = static_cast<uint64_t>(rf.value * scale);
      uint64_t delta = (total >= mPreviousInstructions) ? (total - mPreviousInstructions) : total;
      mPreviousInstructions = total;
      // Per-interval retired instructions; summed over the run = total instructions (cf. cpuUsedAbsolute).
      metrics.emplace_back(delta, metricsNames[CPU_INSTRUCTIONS]);
    }
  }
#endif

  mTimeLastRun = timeNow;
  mPreviousGetrUsage = currentUsage;
  mPreviousGetrUsageChildren = currentUsageChildren;
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
  if (mEnabledMeasurements.at(static_cast<short>(PmMeasurement::Cpu))) {
    auto cpuMetrics = getCpuAndContexts();
    std::move(cpuMetrics.begin(), cpuMetrics.end(), std::back_inserter(metrics));
  }
#ifdef O2_MONITORING_OS_LINUX
  if (mEnabledMeasurements.at(static_cast<short>(PmMeasurement::Mem))) {
    auto memoryMetrics = getMemoryUsage();
    std::move(memoryMetrics.begin(), memoryMetrics.end(), std::back_inserter(metrics));
  }
  if (mEnabledMeasurements.at(static_cast<short>(PmMeasurement::Smaps))) {
    auto smapMetrics = getSmaps();
    std::move(smapMetrics.begin(), smapMetrics.end(), std::back_inserter(metrics));
  }
#endif
  return metrics;
}

std::vector<Metric> ProcessMonitor::makeLastMeasurementAndGetMetrics()
{
  std::vector<Metric> metrics;
#ifdef O2_MONITORING_OS_LINUX
  if (mEnabledMeasurements.at(static_cast<short>(PmMeasurement::Mem))) {
    getMemoryUsage();

    auto avgVmRSS = std::accumulate(mVmRssMeasurements.begin(), mVmRssMeasurements.end(), 0.0) /
                    mVmRssMeasurements.size();

    metrics.emplace_back(avgVmRSS, metricsNames[AVG_RESIDENT_SET_SIZE]);

    auto avgVmSize = std::accumulate(mVmSizeMeasurements.begin(), mVmSizeMeasurements.end(), 0.0) /
                     mVmSizeMeasurements.size();
    metrics.emplace_back(avgVmSize, metricsNames[AVG_VIRTUAL_MEMORY_SIZE]);
  }
#endif
  if (mEnabledMeasurements.at(static_cast<short>(PmMeasurement::Cpu))) {
    // forced: no later call would pick up a delta the rate guard discards here
    auto lastCpuMetrics = getCpuAndContexts(true);
    std::move(lastCpuMetrics.begin(), lastCpuMetrics.end(), std::back_inserter(metrics));

    // A process that ends before the first periodic measurement has no
    // percentages at all (the forced one contributes none), and averaging an
    // empty vector would give NaN.
    if (!mCpuPerctange.empty()) {
      auto avgCpuUsage = std::accumulate(mCpuPerctange.begin(), mCpuPerctange.end(), 0.0) /
                         mCpuPerctange.size();
      metrics.emplace_back(avgCpuUsage, metricsNames[AVG_CPU_USED_PERCENTAGE]);
    }
    uint64_t accumulationOfCpuTimeConsumption = std::accumulate(mCpuMicroSeconds.begin(),
                                                                mCpuMicroSeconds.end(), 0UL);
    metrics.emplace_back(accumulationOfCpuTimeConsumption, metricsNames[ACCUMULATED_CPU_TIME]);
  }
  return metrics;
}

std::vector<std::string> ProcessMonitor::getAvailableMetricsNames()
{
  return {std::begin(metricsNames), std::end(metricsNames)};
}

} // namespace monitoring
} // namespace o2
