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

#define BOOST_TEST_MODULE Test Monitoring ApMon
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

namespace o2
{
namespace monitoring
{
namespace Test
{

BOOST_AUTO_TEST_CASE(simplySendMetric)
{
  boost::filesystem::path configPath = boost::filesystem::canonical(".");
  auto monitoring = MonitoringFactory::Get("apmon://" + configPath.string() + "/ApMon.conf");
  monitoring->send({10, "myCrazyMetric"});
  monitoring->send(Metric{20, "myCrazyMetric"}.addValue(6, "another_value"));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
