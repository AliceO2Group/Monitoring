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
/// \file WebSocket.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "WebSocket.h"
#include "Exceptions/MonitoringException.h"
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

WebSocket::WebSocket(const std::string& hostname, int port, const std::string& token) : mWebSocket(mIoContext)
{
  boost::asio::ip::tcp::resolver resolver{mIoContext};
  auto const results = resolver.resolve(hostname, std::to_string(port));
  boost::asio::connect(mWebSocket.next_layer(), results.begin(), results.end());
  mWebSocket.set_option(beast::websocket::stream_base::decorator(
    [&token](beast::websocket::request_type& req) {
      req.set(beast::http::field::authorization, "Bearer " + token);
    }
  ));
  mWebSocket.handshake(hostname, "/api/live/push/alice_o2");
}

WebSocket::~WebSocket()
{
  mWebSocket.close(beast::websocket::close_code::normal);
}

void WebSocket::send(std::string&& message)
{
  mWebSocket.write(boost::asio::buffer(message));
}

} // namespace transports
} // namespace monitoring
} // namespace o2
