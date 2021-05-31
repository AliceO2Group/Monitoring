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
/// \file TCP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_TCP_H
#define ALICEO2_MONITORING_TRANSPORTS_TCP_H

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

/// \brief Transport that sends string formatted metrics via TCP
class TCP : public TransportInterface
{
 public:
  /// Constructor
  /// \param hostname      InfluxDB instance hostname
  /// \param port          InfluxDB instance port number
  TCP(const std::string& hostname, int port);

  /// Default destructor
  ~TCP() = default;

  /// Sends metric via UDP
  /// \param message   r-value string formated
  void send(std::string&& message) override;

 private:
  /// Boost Asio I/O functionality
  boost::asio::io_service mIoService;

  /// TCP socket
  boost::asio::ip::tcp::socket mSocket;

  /// TCP endpoint interator
  boost::asio::ip::tcp::resolver::iterator mEndpoint;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_TCP_H
