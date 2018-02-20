///
/// \file UDP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "UDP.h"
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

UDP::UDP(const std::string &hostname, int port) :
  mSocket(mIoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
{
    boost::asio::ip::udp::resolver resolver(mIoService);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), hostname, std::to_string(port));
    boost::asio::ip::udp::resolver::iterator resolverInerator = resolver.resolve(query);
    mEndpoint = *resolverInerator;
}

void UDP::send(std::string&& message)
{
  mSocket.send_to(boost::asio::buffer(message, message.size()), mEndpoint);
}

} // namespace transports
} // namespace monitoring
} // namespace o2
