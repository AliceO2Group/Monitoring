// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "../src/UriParser/UriParser.h"

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../src/Backends/Flume.h"


namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(checkJsonEncoding)
{
  std::string url = "flume://localhost:1000";
  auto parsed = http::ParseHttpUrl(url);
  o2::monitoring::backends::Flume flumeBackend(parsed.host, parsed.port);
  o2::monitoring::Metric metric{11, "myCrazyMetric"};
  boost::property_tree::ptree testTree;
  std::stringstream ss;
  ss << flumeBackend.metricToJson(metric);
  read_json(ss, testTree);
  auto headers = testTree.get_child("headers");
  BOOST_CHECK(headers.get<long>("timestamp") > 0);
  BOOST_CHECK_EQUAL(headers.get<std::string>("name"), "myCrazyMetric");
  BOOST_CHECK_EQUAL(headers.get<int>("value_value"), 11);
}
BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  std::string url = "flume://localhost:1000";
  auto parsed = http::ParseHttpUrl(url);
  o2::monitoring::backends::Flume flumeBackend(parsed.host, parsed.port);
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  flumeBackend.send(metric);
}



} // namespace Test
} // namespace monitoring
} // namespace o2
