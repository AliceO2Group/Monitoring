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
/// \file UDP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "UDP.h"
#include <string>
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

UDP::UDP(const std::string& hostname, int port) : mSocket(mIoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
{
  boost::asio::ip::udp::resolver resolver(mIoService);
  boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), hostname, std::to_string(port));
  boost::asio::ip::udp::resolver::iterator resolverInerator = resolver.resolve(query);
  mEndpoint = *resolverInerator;
  MonLogger::Get() << "UDP transport initialized (" << hostname << ":" << port << ")" << MonLogger::End();
}

void UDP::send(std::string&& message)
{
  mSocket.send_to(boost::asio::buffer(message, message.size()), mEndpoint);
}

} // namespace transports
} // namespace monitoring
} // namespace o2
