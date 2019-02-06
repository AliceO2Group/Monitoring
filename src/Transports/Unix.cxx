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
  mSocket(mIoService)
{
    mSocket.connect(boost::asio::local::stream_protocol::endpoint(socketPath));
}

void Unix::send(std::string&& message)
{
  boost::asio::write(mSocket, boost::asio::buffer(message, message.size()));
}
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
} // namespace transports
} // namespace monitoring
} // namespace o2
