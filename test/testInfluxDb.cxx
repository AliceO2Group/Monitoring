// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#define BOOST_TEST_MODULE Test Monitoring InfluxDB
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Monitoring/MonitoringFactory.h"

namespace o2
{
namespace monitoring
{
namespace Test
{
BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  auto monitoring = MonitoringFactory::Get("influxdb-udp://localhost:1000");
  monitoring->send(Metric{10, "myCrazyMetric"});
}

BOOST_AUTO_TEST_CASE(simplySendMetric2)
{
  using namespace std::string_literals;
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
  monitoring->send(Metric{"card"}
    .addValue(40.217773, "temperature")
    .addValue(0, "droppedPackets")
    .addValue(0.768170, "ctpClock")
    .addValue(240.471130, "localClock")
    .addValue(0, "totalPacketsPerSecond")
    .addTag(tags::Key::ID, 3)
    .addTag(tags::Key::Type, tags::Value::CRU)
  );

  monitoring->send(Metric{"link"}
    .addValue("GBT/GBT"s, "gbtMode")
    .addValue("None"s, "loopback")
    .addValue("TTC:CTP"s, "gbtMux")
    .addValue("Continuous"s, "datapathMode")
    .addValue("Disabled"s,  "datapath")
    .addValue(181.370575, "rxFreq")
    .addValue(196.250259, "txFreq")
    .addValue(0, "status")
    .addValue(657.400024, "opticalPower")
    .addTag(tags::Key::CRU, 3)
    .addTag(tags::Key::ID, 3)
  );
}

BOOST_AUTO_TEST_CASE(InfluxDbv2)
{
  auto monitoring = MonitoringFactory::Get("influxdbv2://localhost:9999?org=cern&bucket=test&token=TOKEN");
}

BOOST_AUTO_TEST_CASE(InfluxDbv2)
{
  auto monitoring = MonitoringFactory::Get("influxdbv2://localhost:9999?org=cern&bucket=test&token=TOKEN");
}

} // namespace Test
} // namespace monitoring
} // namespace o2
