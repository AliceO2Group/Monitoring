///
/// \file InfluxBackendUDP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxBackendUDP.h"
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

InfluxBackendUDP::InfluxBackendUDP(boost::asio::io_service& io_service, const std::string &hostname, int port) :
  mIoService(io_service), mSocket(mIoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
{
  boost::asio::ip::udp::resolver resolver(mIoService);
  boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), hostname, std::to_string(port));
  boost::asio::ip::udp::resolver::iterator resolverInerator = resolver.resolve(query);
  mEndpoint = *resolverInerator;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
