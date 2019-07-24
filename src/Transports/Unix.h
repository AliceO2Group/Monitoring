// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file Unix.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_UNIX_H
#define ALICEO2_MONITORING_TRANSPORTS_UNIX_H

#include "TransportInterface.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
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

/// \brief Transport that sends string formatted metrics via Unix datagram socket
class Unix : public TransportInterface
{
 public:
  /// \param hostname
  /// \param port
  Unix(const std::string& socketPath);

  /// Default destructor
  ~Unix() = default;

  /// \param message   r-value string formated
  void send(std::string&& message) override;

 private:
  /// Boost Asio I/O functionality
  boost::asio::io_service mIoService;
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
  /// Unix socket
  boost::asio::local::datagram_protocol::socket mSocket;

  /// Unix endpoint
  boost::asio::local::datagram_protocol::endpoint mEndpoint;
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_UNIX_H
