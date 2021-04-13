// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#define BOOST_TEST_MODULE Monitoring StdoutBackends
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Monitoring/MonitoringFactory.h"

namespace o2
{
namespace monitoring
{
namespace Test
{

BOOST_AUTO_TEST_CASE(parseInfluxDBStdOutDataPoints)
{
  using namespace std::string_literals;
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");

  std::stringstream coutRedirect;
  std::streambuf* coutBuffer = std::cout.rdbuf(coutRedirect.rdbuf());

  monitoring->send(Metric{"card"}
    .addValue(40.217773, "temperature")
    .addValue(0, "droppedPackets")
    .addValue(0.768170, "ctpClock")
    .addValue(240.471130, "localClock")
    .addValue(0, "totalPacketsPerSecond")
    .addTag(tags::Key::ID, 3)
    .addTag(tags::Key::Type, tags::Value::CRU)
  );
  std::string expected1 = "card,hostname=pcaldadam2.local,id=3,type=CRU temperature=40.2178,droppedPackets=0i,ctpClock=0.76817,localClock=240.471,totalPacketsPerSecond=0i 1618241730312076000\n";
  BOOST_CHECK(expected1.compare(coutRedirect.str()));
  coutRedirect.str(std::string());

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
  std::string expected2 = R"(link,hostname=pcaldadam2.local,CRU=3,id=3 gbtMode="GBT/GBT",loopback="None",gbtMux="TTC:CTP",datapathMode="Continuous",datapath="Disabled",rxFreq=181.371,txFreq=196.25,status=0i,opticalPower=657.4 1618241730312104000)";
  BOOST_CHECK(expected2.compare(coutRedirect.str()));

  std::cout.rdbuf(coutBuffer);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
