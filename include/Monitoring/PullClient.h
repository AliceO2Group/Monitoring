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

///
/// \file PullClient.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_PULLCLIENT_H
#define ALICEO2_MONITORING_CORE_PULLCLIENT_H

#include <vector>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief PullClient pure virtual interface
///
/// Interface that allows to send a metric to remote backend.
/// In addition, default tagset (for all handled metrics) can be created.
class PullClient
{
 public:
  /// Default destructor
  virtual ~PullClient() = default;
  virtual std::vector<std::string> pull() = 0;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_PULLCLIENT_H
