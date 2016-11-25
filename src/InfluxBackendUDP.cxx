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

InfluxBackendUDP::InfluxBackendUDP(const std::string &hostname, int port) :
  mSocket(mIoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
{
  boost::asio::ip::udp::resolver resolver(mIoService);
  boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), hostname, std::to_string(port));
  boost::asio::ip::udp::resolver::iterator resolverInerator = resolver.resolve(query);
  mEndpoint = *resolverInerator;
  MonInfoLogger::GetInstance() << "InfluxDB via UDP backend enabled" << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfluxBackendUDP::sendUDP(std::string value, const std::string& name, const std::string& entity,
  const unsigned long timestamp)
{
  std::string escapedName = name;
  // escape space in name for InluxDB
  boost::replace_all(escapedName, " ", "\\ ");

  // preparing post data
  std::stringstream convert;
  convert << escapedName << ",entity=" << entity << " value=" << value << " " << timestamp;
  std::string lineMessage = convert.str();
  mSocket.send_to(boost::asio::buffer(lineMessage, lineMessage.size()), mEndpoint);
  MonInfoLogger::GetInstance() << "InfluxDB via UDP : metric " <<  name << ", packet sent"
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

inline unsigned long InfluxBackendUDP::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void InfluxBackendUDP::send(int value, const std::string& name, const std::string& entity,
  const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  sendUDP(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

void InfluxBackendUDP::send(double value, const std::string& name, const std::string& entity,
  const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  sendUDP(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

void InfluxBackendUDP::send(std::string value, const std::string& name, const std::string& entity,
  const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  value.insert(value.begin(), '"');
  value.insert(value.end(), '"');
  sendUDP(value, name, entity, convertTimestamp(timestamp));
}

void InfluxBackendUDP::send(uint32_t value, const std::string& name, const std::string& entity, 
  const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  sendUDP(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
