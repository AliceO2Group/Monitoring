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

void InfluxBackendUDP::sendUDP(std::string value, std::string name,
  const unsigned long timestamp)
{
  escape(name);
  
  std::stringstream convert;
  convert << name << "," << tagSet << " value=" << value << " " << timestamp;
  std::string lineMessage = convert.str();
  mSocket.send_to(boost::asio::buffer(lineMessage, lineMessage.size()), mEndpoint);
}

inline unsigned long InfluxBackendUDP::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void InfluxBackendUDP::escape(std::string& escaped)
{
  boost::replace_all(escaped, ",", "\\,");
  boost::replace_all(escaped, "=", "\\=");
  boost::replace_all(escaped, " ", "\\ ");
}

void InfluxBackendUDP::send(const Metric& metric)
{
  std::string value = boost::lexical_cast<std::string>(metric.getValue());
  if (metric.getType() == MetricType::STRING) {
    escape(value);
    value.insert(value.begin(), '"');
    value.insert(value.end(), '"');
  }
  sendUDP(value, metric.getName(), convertTimestamp(metric.getTimestamp()));
}

void InfluxBackendUDP::addGlobalTag(std::string name, std::string value)
{
  escape(name); escape(value);
  if (!tagSet.empty()) tagSet += ",";
  tagSet += name + "=" + value;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
