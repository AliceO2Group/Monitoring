///
/// \file TCP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "TCP.h"
#include <string>
#include <iostream>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring transports
namespace Transports
{

TCP::TCP(const std::string &hostname, int port) :
  mSocket(mIoService)
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
    throw boost::system::system_error(error);
  } 
}

void TCP::send(std::string&& message)
{
  mSocket.send(boost::asio::buffer(message));
}

void TCP::read(/*unsigned int bytes*/) {
  // dummy read!
  for (;;) {
    boost::system::error_code error;
    boost::array<char, 128> buf;
    size_t len = mSocket.read_some(boost::asio::buffer(buf), error);
    if (error == boost::asio::error::eof) {
      break;
    }
    std::cout.write(buf.data(), len);
  }
}

} // namespace Transports
} // namespace Monitoring
} // namespace AliceO2
