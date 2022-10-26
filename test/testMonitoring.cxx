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

#define BOOST_TEST_MODULE Monitoring Interface
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Monitoring/MonitoringFactory.h"

using namespace std::string_literals;

namespace o2
{
namespace monitoring
{
namespace Test
{

auto monitoring = MonitoringFactory::Get("influxdb-stdout://");
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

void removeTimestamp(std::string& metric)
{
  metric.erase(metric.rfind(' '));
}


BOOST_AUTO_TEST_CASE(parseDataPoints)
{
  enableRedirect();

  monitoring->send(Metric{"card"}
    .addValue(40.217773, "temperature")
    .addValue(0, "droppedPackets")
    .addValue(0.768170, "ctpClock")
    .addValue(240.471130, "localClock")
    .addValue(0, "totalPacketsPerSecond")
    .addTag(tags::Key::ID, 3)
    .addTag(tags::Key::Type, tags::Value::CRU)
  );
  std::string expected1 = "card,id=3,type=CRU temperature=40.2178,droppedPackets=0i,ctpClock=0.76817,localClock=240.471,totalPacketsPerSecond=0i";
  std::string returned1 = coutRedirect.str();

  disableRedirect();
  removeTimestamp(returned1);
  BOOST_CHECK_EQUAL(expected1, returned1);

  enableRedirect();
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
  std::string expected2 = R"(link,CRU=3,id=3 gbtMode="GBT/GBT",loopback="None",gbtMux="TTC:CTP",datapathMode="Continuous",datapath="Disabled",rxFreq=181.371,txFreq=196.25,status=0i,opticalPower=657.4)";
  std::string returned2 = coutRedirect.str();
  disableRedirect();
  removeTimestamp(returned2);
  BOOST_CHECK_EQUAL(expected2, returned2);
}

BOOST_AUTO_TEST_CASE(testSettingRun)
{
  enableRedirect();

  monitoring->setRunNumber(1234);
  monitoring->send(Metric{4321, "test"});
  std::string expected = "test,run=1234 value=4321i";
  std::string returned = coutRedirect.str();

  disableRedirect();
  removeTimestamp(returned);
  BOOST_CHECK_EQUAL(expected, returned);
}

BOOST_AUTO_TEST_CASE(testGlobalTags)
{
  enableRedirect();

  monitoring->addGlobalTag("custom_name", "Monitoring");
  monitoring->addGlobalTag(tags::Key::Name, tags::Value::Readout);
  monitoring->send(Metric{4321, "test"});
  std::string expected = "test,custom_name=Monitoring,name=Readout,run=1234 value=4321i";
  std::string returned = coutRedirect.str();

  disableRedirect();
  removeTimestamp(returned);
  BOOST_CHECK_EQUAL(expected, returned);
}

BOOST_AUTO_TEST_CASE(addHostnameTag)
{
  enableRedirect();

  monitoring->setRunNumber(1234);
  monitoring->addHostnameTag();
  monitoring->send(Metric{4321, "test"});
  std::string returned = coutRedirect.str();

  disableRedirect();
  BOOST_CHECK(returned.find("hostname=") != std::string::npos);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
