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
/// \file TransportInterface.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
#define ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H

#include "../Exceptions/MonitoringException.h"
#include <string>
#include <vector>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport interface for backends
class TransportInterface
{
 public:
  TransportInterface() = default;

  virtual ~TransportInterface() = default;

  /// Sends metric via given transport
  /// \param message   r-value to string formatted metric
  virtual void send(std::string&& message) = 0;

  /// Receives metric via given transport
  /// \return    List of messages
  virtual std::vector<std::string> receive() {
    throw MonitoringException("Transport", "This transport does not implement receiving metrics");
  }
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
