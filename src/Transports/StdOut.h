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
/// \file StdOut.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_STDOUT_H
#define ALICEO2_MONITORING_TRANSPORTS_STDOUT_H

#include "TransportInterface.h"

#include <chrono>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport that sends string formatted metrics via StdOut
class StdOut : public TransportInterface
{
 public:
  /// Constructor
  StdOut() = default;

  /// Default destructor
  ~StdOut() = default;

  /// Sends metric via StdOut
  /// \param message   r-value string formated
  void send(std::string&& message) override;

  /// Overload stream operator
  template <typename T>
  StdOut& operator<<(const T& msg);
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_STDOUT_H
