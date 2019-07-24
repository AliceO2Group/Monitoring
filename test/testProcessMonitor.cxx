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
  processMonitor.getCpuAndContexts();
#ifdef O2_MONITORING_OS_LINUX
  processMonitor.getMemoryUsage();
#endif
}

BOOST_AUTO_TEST_CASE(monitorProcess)
{
  auto monitoring = o2::monitoring::MonitoringFactory::Get("stdout://");
  monitoring->enableProcessMonitoring(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(2100));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
