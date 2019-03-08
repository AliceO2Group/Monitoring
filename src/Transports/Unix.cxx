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
/// \file Unix.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Unix.h"
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
Unix::Unix(const std::string &socketPath) :
  mSocket(mIoService), mEndpoint(socketPath)
{
  mSocket.open();
}

void Unix::send(std::string&& message)
{
  mSocket.send_to(boost::asio::buffer(message, message.size()), mEndpoint);
}
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
} // namespace transports
} // namespace monitoring
} // namespace o2
