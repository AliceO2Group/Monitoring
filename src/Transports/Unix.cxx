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
