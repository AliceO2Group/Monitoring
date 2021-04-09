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
/// \file TCP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "TCP.h"
#include <string>
#include <iostream>
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

TCP::TCP(const std::string& hostname, int port) : mSocket(mIoService)
{
  boost::asio::ip::tcp::resolver resolver(mIoService);
  boost::asio::ip::tcp::resolver::query query(hostname, std::to_string(port));
  boost::asio::ip::tcp::resolver::iterator resolverIterator = resolver.resolve(query);

  boost::asio::ip::tcp::resolver::iterator end;
  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && resolverIterator != end) {
    mSocket.close();
    mSocket.connect(*resolverIterator++, error);
  }
  if (error) {
    throw MonitoringException("TCP connection", error.message());
  }
  MonLogger::Get(Severity::Info) << "TCP transport initialized (" << hostname << ":" << port << ")" << MonLogger::End();
}

void TCP::send(std::string&& message)
{
  try {
    mSocket.send(boost::asio::buffer(message));
  } catch (const boost::system::system_error& e) {
    MonLogger::Get() << "TCP / " << e.what() << MonLogger::End();
  }
}

} // namespace transports
} // namespace monitoring
} // namespace o2
