///
/// \file Zabbix.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Zabbix.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include "../Transports/TCP.h"
#include "../Exceptions/MonitoringInternalException.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

Zabbix::Zabbix(const http::url& uri)
 : socketHostname(uri.host), socketPort(uri.port)
{ 
  MonLogger::Get() << "Zabbix/TCP backend initialized"
                       << " ("<< uri.host << ":" << uri.port << ")" << MonLogger::End();
}

inline std::string Zabbix::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  std::string converted = std::to_string(
    std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count());
  converted.erase(converted.begin()+10, converted.end());
  return converted;
}

void Zabbix::send(const Metric& metric)
{
  // create JSON payload
  boost::property_tree::ptree dataArray, data;
  data.put<std::string>("host", hostname);
  data.put<std::string>("key", metric.getName());
  data.put<std::string>("value", boost::lexical_cast<std::string>(metric.getValue()));
  data.put<std::string>("clock", convertTimestamp(metric.getTimestamp()));
  dataArray.push_back(std::make_pair("", data));
  sendOverTcp(createMessage(dataArray));
}

void Zabbix::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  boost::property_tree::ptree dataArray;
  for (auto& metric : metrics) {
    boost::property_tree::ptree data;
    data.put<std::string>("host", hostname);
    data.put<std::string>("key", metric.getName());
    data.put<std::string>("value", boost::lexical_cast<std::string>(metric.getValue()));
    data.put<std::string>("clock", convertTimestamp(metric.getTimestamp()));
    dataArray.push_back(std::make_pair("", data));
  }
  sendOverTcp(createMessage(dataArray));
}

std::string Zabbix::createMessage(const boost::property_tree::ptree& dataArray) {
  boost::property_tree::ptree request;
  request.put<std::string>("request", "sender data");
  request.add_child("data", dataArray);

  std::stringstream ss;
  write_json(ss, request);

  std::string message = ss.str();
  message.erase(std::remove_if( message.begin(), message.end(),
      [](char c){ return (c =='\r' || c =='\t' || c == ' ' || c == '\n');}), message.end() );
  message.insert(18, " ");

  uint32_t length = message.length();
  std::vector<unsigned char> header = {
    'Z', 'B', 'X', 'D', '\x01',
    static_cast<unsigned char>(length & 0xFF),
    static_cast<unsigned char>((length >> 8) & 0x00FF),
    static_cast<unsigned char>((length >> 16) & 0x0000FF),
    static_cast<unsigned char>((length >> 24) & 0x000000FF),
    '\x00','\x00','\x00','\x00'
  };
  return std::string(header.begin(), header.end()) + message;
}

void Zabbix::sendOverTcp(std::string&& message) {
  try {
    std::unique_ptr<Transports::TCP> transport = std::make_unique<Transports::TCP>(socketHostname, socketPort);
    transport->send(std::move(message));
  } catch (MonitoringInternalException&) {
  }
}

void Zabbix::addGlobalTag(std::string name, std::string value)
{
  if (name == "hostname") {
    hostname = value;
  }
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
