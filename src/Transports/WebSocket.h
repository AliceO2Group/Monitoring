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
/// \file WebSocket.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_WEBSOCKET_H
#define ALICEO2_MONITORING_TRANSPORTS_WEBSOCKET_H

#include "TransportInterface.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

namespace beast = boost::beast;

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport that sends string formatted metrics via WebSocket
class WebSocket : public TransportInterface
{
 public:
  /// Constructor
  /// \param hostname      Grafana host
  /// \param port          Grafana port
  /// \param token         Grafana API token
  /// \param stream        Name of WebSocket stream
  WebSocket(const std::string& hostname, int port, const std::string& token, const std::string& stream = "alice_o2");

  /// Gracefull disconnect
  ~WebSocket();

  /// Sends metric via UDP
  /// \param message   r-value string formated
  void send(std::string&& message) override;

  /// Read control frames from socket
  void read();
 private:
  /// IO context
  boost::asio::io_context mIoContext;

  /// Websocket stream
  beast::websocket::stream<boost::asio::ip::tcp::socket> mWebSocket;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_WEBSOCKET_H
