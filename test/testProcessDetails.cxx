// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "../src/ProcessDetails.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessDetails
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <regex>

namespace o2
{
namespace monitoring
{
namespace Test
{

o2::monitoring::ProcessDetails processDetails;

BOOST_AUTO_TEST_CASE(checkPid)
{
  BOOST_CHECK(processDetails.getPid() > 1);
}

BOOST_AUTO_TEST_CASE(checkProcessName)
{
  std::string referenceProcessName = "testProcessDetails";
  BOOST_CHECK(referenceProcessName == processDetails.getProcessName());
}

BOOST_AUTO_TEST_CASE(checkHostname)
{
  const std::regex hostnameRE("^(?!-)[A-Za-z0-9-]+([\\-\\.]{1}[a-z0-9]+)*\\.[A-Za-z]{2,6}$");
  std::string hostname = processDetails.getHostname();
  BOOST_CHECK(regex_match(hostname, hostnameRE));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
