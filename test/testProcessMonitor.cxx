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

BOOST_AUTO_TEST_CASE(createProcessMonitor)
{
  o2::monitoring::ProcessMonitor processMonitor;
  processMonitor.getPerformanceMetrics();
}

BOOST_AUTO_TEST_CASE(monitorProcess)
{
  auto monitoring = o2::monitoring::MonitoringFactory::Get("stdout://");
  monitoring->enableProcessMonitoring(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(2100));
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
