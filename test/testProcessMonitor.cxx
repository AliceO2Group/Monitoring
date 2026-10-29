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

#include "Monitoring/ProcessMonitor.h"
#include "Monitoring/MonitoringFactory.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessMonitor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <thread>

namespace o2
{
namespace monitoring
{
namespace Test
{

/// Runs process monitoring for the given duration, and returns the names of the published metrics.
/// \param enable  call enabling process monitoring on the monitoring object
std::vector<std::string> getPublishedMetrics(std::function<void(Monitoring&)> enable, std::chrono::milliseconds duration)
{
  std::stringstream output;
  auto coutBuffer = std::cout.rdbuf(output.rdbuf());
  {
    auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
    enable(*monitoring);
    std::this_thread::sleep_for(duration);
  }
  std::cout.rdbuf(coutBuffer);

  std::vector<std::string> names;
  for (std::string line; std::getline(output, line);) {
    names.push_back(line.substr(0, line.find_first_of(" ,")));
  }
  return names;
}

/// Checks published metrics against the ones defined for the given measurements:
/// all non-optional ones must be there, and nothing else.
void checkPublishedMetrics(const std::vector<std::string>& published, const std::vector<PmMeasurement>& measurements)
{
  std::set<std::string> required;
  std::set<std::string> allowed;
  for (const auto& measurement : measurements) {
    for (const auto& metric : ProcessMonitor::getMetricsInfo(measurement)) {
      allowed.insert(metric.name);
      if (!metric.optional) {
        required.insert(metric.name);
      }
    }
  }
  std::set<std::string> publishedSet(published.begin(), published.end());
  for (const auto& name : published) {
    BOOST_CHECK_MESSAGE(allowed.count(name), "unexpected metric: " << name);
  }
  for (const auto& name : required) {
    BOOST_CHECK_MESSAGE(publishedSet.count(name), "missing metric: " << name);
  }
}

// Running slightly more than the 1s interval gives one periodic measurement and the final one,
// so that all non-optional metrics are published.
constexpr std::chrono::milliseconds runDuration(1500);

BOOST_AUTO_TEST_CASE(monitorProcessDefault)
{
  auto published = getPublishedMetrics([](Monitoring& m) { m.enableProcessMonitoring(1); }, runDuration);
  checkPublishedMetrics(published, {PmMeasurement::Cpu, PmMeasurement::Mem, PmMeasurement::Smaps});
}

BOOST_AUTO_TEST_CASE(monitorProcessCpuOnly)
{
  auto published = getPublishedMetrics([](Monitoring& m) { m.enableProcessMonitoring(1, {PmMeasurement::Cpu}); }, runDuration);
  checkPublishedMetrics(published, {PmMeasurement::Cpu});
}

BOOST_AUTO_TEST_CASE(monitorProcessCpuMem)
{
  auto published = getPublishedMetrics([](Monitoring& m) { m.enableProcessMonitoring(1, {PmMeasurement::Cpu, PmMeasurement::Mem}); }, runDuration);
  checkPublishedMetrics(published, {PmMeasurement::Cpu, PmMeasurement::Mem});
}

BOOST_AUTO_TEST_CASE(monitorProcessMetricName)
{
  auto vec = o2::monitoring::ProcessMonitor::getAvailableMetricsNames();
  BOOST_CHECK_EQUAL(vec.size(), o2::monitoring::ProcessMonitor::AVAILABLE_METRICS_SIZE);
  BOOST_CHECK_EQUAL(vec[o2::monitoring::ProcessMonitor::AVG_CPU_USED_PERCENTAGE], "averageCpuUsedPercentage");
}

BOOST_AUTO_TEST_CASE(monitorProcessMetricsInfo)
{
  // each metric belongs to one measurement, and is one of the available metrics
  auto available = ProcessMonitor::getAvailableMetricsNames();
  std::set<std::string> seen;
  for (auto measurement : {PmMeasurement::Cpu, PmMeasurement::Mem, PmMeasurement::Smaps}) {
    for (const auto& metric : ProcessMonitor::getMetricsInfo(measurement)) {
      BOOST_CHECK(metric.measurement == measurement);
      BOOST_CHECK_MESSAGE(std::find(available.begin(), available.end(), metric.name) != available.end(), "unknown metric: " << metric.name);
      BOOST_CHECK_MESSAGE(seen.insert(metric.name).second, "metric in several measurements: " << metric.name);
    }
  }
  BOOST_CHECK(!ProcessMonitor::getMetricsInfo(PmMeasurement::Cpu).empty());
}

} // namespace Test
} // namespace monitoring
} // namespace o2
