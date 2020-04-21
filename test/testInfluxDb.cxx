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
/*
BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  auto monitoring = MonitoringFactory::Get("influxdb-udp://localhost:1000");
  monitoring->send(Metric{10, "myCrazyMetric"});
}

BOOST_AUTO_TEST_CASE(simplySendMetric2)
{
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
  monitoring->send(Metric{10, "myCrazyMetric"});
}
*/
BOOST_AUTO_TEST_CASE(InfluxDbv2)
{
  auto monitoring = MonitoringFactory::Get("influxdbv2://localhost:9999?org=cern&bucket=test&token=c1WZvMvFK9KHrRAD68Ou8U4d9hljQm_haOjOL6Xn4oPM89nDZiJ5mwGrO-Z7YspoFnQP-4BIyhomf_Yq111fVg==");
  monitoring->send(Metric{10, "myCrazyMetric"});
}

} // namespace Test
} // namespace monitoring
} // namespace o2
