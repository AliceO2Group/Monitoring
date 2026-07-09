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

#include <algorithm>
#include <string>
#include <vector>

namespace o2
{
namespace monitoring
{
namespace Test
{

std::stringstream coutRedirect;
std::streambuf* coutBuffer;

void enableRedirect()
{
  coutBuffer = std::cout.rdbuf(coutRedirect.rdbuf());
}

void disableRedirect()
{
  coutRedirect.str(std::string());
  std::cout.rdbuf(coutBuffer);
}

// Collects the metric names emitted on the redirected stdout. Every metric line
// starts with the metric (measurement) name followed by a space.
static std::vector<std::string> collectEmittedMetricNames()
{
  std::istringstream returned(coutRedirect.str());
  disableRedirect();
  std::vector<std::string> names;
  for (std::string line; std::getline(returned, line);) {
    if (line.empty()) {
      continue;
    }
    names.push_back(line.substr(0, line.find(' ')));
  }
  return names;
}

// Checks that every emitted metric is one declared in ProcessMonitor (metricsNames /
// the enum), so adding a new metric never requires updating a hardcoded list here.
static void checkAllMetricsAreKnown(const std::vector<std::string>& emitted)
{
  const auto available = o2::monitoring::ProcessMonitor::getAvailableMetricsNames();
  for (const auto& name : emitted) {
    BOOST_CHECK_MESSAGE(std::find(available.begin(), available.end(), name) != available.end(),
                        "Emitted metric '" << name << "' is not declared in ProcessMonitor");
  }
}

BOOST_AUTO_TEST_CASE(monitorProcessDefaultCount)
{
  {
    auto monitoring = o2::monitoring::MonitoringFactory::Get("influxdb-stdout://");
    monitoring->enableProcessMonitoring(1);
    enableRedirect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  const auto emitted = collectEmittedMetricNames();
  checkAllMetricsAreKnown(emitted);
  BOOST_CHECK_GT(emitted.size(), 0u);
}

BOOST_AUTO_TEST_CASE(monitorProcessCpuOnly)
{
  {
    auto monitoring = o2::monitoring::MonitoringFactory::Get("influxdb-stdout://");
    monitoring->enableProcessMonitoring(1, {PmMeasurement::Cpu});
    enableRedirect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  const auto emitted = collectEmittedMetricNames();
  checkAllMetricsAreKnown(emitted);
  BOOST_CHECK_GT(emitted.size(), 0u);

  // With only CPU enabled, no memory metric should be emitted (name taken from
  // ProcessMonitor's declaration rather than hardcoded).
  const auto available = o2::monitoring::ProcessMonitor::getAvailableMetricsNames();
  const auto& residentSetSize = available[o2::monitoring::ProcessMonitor::RESIDENT_SET_SIZE];
  BOOST_CHECK(std::find(emitted.begin(), emitted.end(), residentSetSize) == emitted.end());
}

BOOST_AUTO_TEST_CASE(monitorProcessAll)
{
  {
    auto monitoring = o2::monitoring::MonitoringFactory::Get("influxdb-stdout://");
    monitoring->enableProcessMonitoring(1, {PmMeasurement::Cpu, PmMeasurement::Mem});
    enableRedirect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  const auto emitted = collectEmittedMetricNames();
  checkAllMetricsAreKnown(emitted);
  BOOST_CHECK_GT(emitted.size(), 0u);

  // CPU and memory are enabled but not Smaps, so no proportional-set-size metric
  // should be emitted (name taken from ProcessMonitor's declaration).
  const auto available = o2::monitoring::ProcessMonitor::getAvailableMetricsNames();
  const auto& proportionalSetSize = available[o2::monitoring::ProcessMonitor::PSS];
  BOOST_CHECK(std::find(emitted.begin(), emitted.end(), proportionalSetSize) == emitted.end());
}

BOOST_AUTO_TEST_CASE(monitorProcessMetricName)
{
  auto vec = o2::monitoring::ProcessMonitor::getAvailableMetricsNames();
  BOOST_CHECK_EQUAL(vec.size(), o2::monitoring::ProcessMonitor::AVAILABLE_METRICS_SIZE);
  BOOST_CHECK_EQUAL(vec[o2::monitoring::ProcessMonitor::AVG_CPU_USED_PERCENTAGE], "averageCpuUsedPercentage");
}

} // namespace Test
} // namespace monitoring
} // namespace o2
