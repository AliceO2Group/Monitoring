// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "Monitoring/ProcessMonitor.h"
#include "Monitoring/MonitoringFactory.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessMonitor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

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


BOOST_AUTO_TEST_CASE(monitorProcess)
{
  std::array<std::string, 14> names = {"memoryUsagePercentage", "virtualMemorySize", "residentSetSize",
                                       "cpuUsedPercentage", "involuntaryContextSwitches", "voluntaryContextSwitches", "cpuUsedAbsolute",
                                       "averageResidentSetSize", "averageVirtualMemorySize", "averageCpuUsedPercentage",
                                       "cpuTimeConsumedByProcess", "proportionalSetSize", "memoryPrivateClean", "memoryPrivateDirty"};
  {
    auto monitoring = o2::monitoring::MonitoringFactory::Get("influxdb-stdout://");
    monitoring->enableProcessMonitoring(1);
    enableRedirect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  std::istringstream returned(coutRedirect.str());
  disableRedirect();
  unsigned short int countMetrics = 0;
  for (std::string line; std::getline(returned, line); ) {
    BOOST_CHECK(std::find(names.begin(), names.end(), line.substr(0, line.find(','))) != names.end());
    countMetrics++;
  }
  // On linux 14 and macOS 6
  BOOST_CHECK(countMetrics == 14 || countMetrics == 6);
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
