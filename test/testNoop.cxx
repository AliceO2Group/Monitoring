// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#define BOOST_TEST_MODULE Test Monitoring Flume
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../src/Backends/Noop.h"

namespace o2
{
namespace monitoring
{
namespace Test
{

BOOST_AUTO_TEST_CASE(noopSendMetric)
{
  o2::monitoring::backends::Noop noopBackend{};
  o2::monitoring::Metric metric{10, "myCrazyMetric"};
  noopBackend.send(metric);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
