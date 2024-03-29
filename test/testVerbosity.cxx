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

#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE Monitoring Verbosityy
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "Monitoring/MonitoringFactory.h"

namespace o2
{
namespace monitoring
{
namespace Test
{

using Monitoring = o2::monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(verbosity)
{
  std::string ilUrl = "stdout://";
  auto il = Monitoring::GetBackend(ilUrl);
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<Verbosity>::type>(il->getVerbosity()), 2);

  std::string influxUrl = "influxdb-udp://127.0.0.1:1234";
  auto influx = Monitoring::GetBackend(influxUrl);
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<Verbosity>::type>(influx->getVerbosity()), 1);

  std::string influxDebugUrl = "influxdb-udp://127.0.0.1:1234/debug";
  auto influxDebug = Monitoring::GetBackend(influxDebugUrl);
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<Verbosity>::type>(influxDebug->getVerbosity()), 2);

  std::string ilProdUrl = "stdout:///info";
  auto ilProd = Monitoring::GetBackend(ilProdUrl);
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<Verbosity>::type>(ilProd->getVerbosity()), 1);
}

} // namespace Test
} // namespace monitoring
} // namespace o2
