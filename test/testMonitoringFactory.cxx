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

#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE Monitoring Factory
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>

#include "../include/Monitoring/MonitoringFactory.h"

namespace o2
{
namespace monitoring
{
namespace Test
{

using Monitoring = o2::monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(influxdbUdp)
{
  MonitoringFactory::Get("influxdb-udp://localhost:1000");
}

BOOST_AUTO_TEST_CASE(InfluxDbv2)
{
  MonitoringFactory::Get("influxdbv2://localhost:9999?org=cern&bucket=test&token=TOKEN");
}

BOOST_AUTO_TEST_CASE(StdOut)
{
  MonitoringFactory::Get("stdout://");
}

BOOST_AUTO_TEST_CASE(Noop)
{
  MonitoringFactory::Get("no-op://");
}

#ifdef O2_MONITORING_WITH_APPMON
BOOST_AUTO_TEST_CASE(ApMon)
{
  std::filesystem::path configPath = std::filesystem::canonical(".");
  auto Monitoring = MonitoringFactory::Get("apmon://" + configPath.string() + "/ApMon.conf");
  monitoring->send({10, "myCrazyMetric"});
}
#endif

} // namespace Test
} // namespace monitoring
} // namespace o2
