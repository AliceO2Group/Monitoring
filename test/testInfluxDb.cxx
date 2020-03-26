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
#include "../src/UriParser/UriParser.h"
#include "../src/Transports/UDP.h"
#include "../src/Transports/StdOut.h"
#include "../src/Backends/InfluxDB.h"

namespace o2
{
namespace monitoring
{
namespace Test
{

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  std::string url = "influxdb-udp://localhost:1000";
  auto parsed = http::ParseHttpUrl(url);
  auto transport = std::make_unique<transports::UDP>(parsed.host, parsed.port);
  o2::monitoring::backends::InfluxDB influxBackend(std::move(transport));
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  influxBackend.send(metric);
}

BOOST_AUTO_TEST_CASE(simplySendMetric2)
{
  auto transport = std::make_unique<transports::StdOut>();
  o2::monitoring::backends::InfluxDB influxBackend(std::move(transport));
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  influxBackend.send(metric);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
