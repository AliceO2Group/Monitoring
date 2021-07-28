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
/// \file UDP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_UDP_H
#define ALICEO2_MONITORING_TRANSPORTS_UDP_H

#include "TransportInterface.h"

#include <boost/asio.hpp>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport that sends string formatted metrics via UDP
class UDP : public TransportInterface
{
 public:
  /// Constructor
  /// \param hostname      InfluxDB instance hostname
  /// \param port          InfluxDB instance port number
  UDP(const std::string& hostname, int port);

  /// Default destructor
  ~UDP() = default;

  /// Sends metric via UDP
  /// \param message   r-value string formated
  void send(std::string&& message) override;

 private:
  /// Boost Asio I/O functionality
  boost::asio::io_service mIoService;

  /// UDP socket
  boost::asio::ip::udp::socket mSocket;

  /// UDP endpoint
  boost::asio::ip::udp::endpoint mEndpoint;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_UDP_H
