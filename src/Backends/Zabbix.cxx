///
/// \file Zabbix.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Zabbix.h"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
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

Zabbix::Zabbix(const std::string &hostname, int port)
{ 
  transport = std::make_unique<Transports::TCP>(hostname, port);
  MonLogger::Get() << "Zabbix/TCP backend initialized"
                       << " ("<< hostname << ":" << port << ")" << MonLogger::End();
}

std::string Zabbix::metricToZabbix(const Metric& metric)
{
  // create JSON payload
  boost::property_tree::ptree request, dataArray, data;
  //data.put<std::string>("clock", std::to_string(convertTimestamp(metric.getTimestamp())));
  data.put<std::string>("host", hostname);
  data.put<std::string>("key", metric.getName());
  data.put<std::string>("value", boost::lexical_cast<std::string>(metric.getValue()));

  dataArray.push_back(std::make_pair("", data));
  request.put<std::string>("request", "sender data");
  request.add_child("data", dataArray);

  std::stringstream ss; 
  write_json(ss, request);
  
  std::string noWhiteSpaces = ss.str();
  noWhiteSpaces.erase(std::remove_if( noWhiteSpaces.begin(), noWhiteSpaces.end(), 
      [](char c){ return (c =='\r' || c =='\t' || c == ' ' || c == '\n');}), noWhiteSpaces.end() );
  noWhiteSpaces.insert(18, " ");

  // prepare Zabbix header
  uint32_t length = noWhiteSpaces.length();
  std::vector<unsigned char> header = {
    'Z', 'B', 'X', 'D', '\x01',
    static_cast<unsigned char>(length & 0xFF),
    static_cast<unsigned char>((length >> 8) & 0x00FF),
    static_cast<unsigned char>((length >> 16) & 0x0000FF),
    static_cast<unsigned char>((length >> 24) & 0x000000FF),
    '\x00','\x00','\x00','\x00'
  };

  return std::string(header.begin(), header.end()) + noWhiteSpaces;
}

void Zabbix::send(const Metric& metric) {  
  try {
    transport->send(metricToZabbix(metric));
  } catch (MonitoringInternalException&) {
  }
  //transport->read();
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
