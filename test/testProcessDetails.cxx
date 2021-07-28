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

#include "../src/ProcessDetails.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessDetails
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

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

} // namespace Test
} // namespace monitoring
} // namespace o2
