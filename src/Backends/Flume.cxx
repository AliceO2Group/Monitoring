///
/// \file Flume.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Flume.h"
#include <string>
#include "../Transports/UDP.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

Flume::Flume(const std::string &hostname, int port)
{
  mTransport = std::make_unique<Transports::UDP>(hostname, port);
  MonLogger::Get() << "Flume/UDP backend initialized"
                       << " ("<< hostname << ":" << port << ")" << MonLogger::End();
}

std::string Flume::metricToJson(const Metric& metric)
{
    boost::property_tree::ptree event;
    boost::property_tree::ptree header = globalHeader;
    header.put<std::string>("timestamp", std::to_string(convertTimestamp(metric.getTimestamp())));
    header.put<std::string>("name", metric.getName());
    header.put<std::string>("value", boost::lexical_cast<std::string>(metric.getValue()));
    for (const auto& tag : metric.getTags()) {
      header.put<std::string>(tag.name, tag.value);
    }   
    event.push_back(std::make_pair("headers", header));
    event.put<std::string>("body", boost::lexical_cast<std::string>(metric.getValue()));
    std::stringstream ss; 
    write_json(ss, event);
    std::string s = ss.str();
    s.erase(std::remove_if( s.begin(), s.end(), 
      [](char c){ return (c =='\r' || c =='\t' || c == ' ' || c == '\n');}), s.end() );
    return s;
}

void Flume::send(const Metric& metric)
{
    mTransport->send(metricToJson(metric));
}

inline unsigned long Flume::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void Flume::addGlobalTag(std::string name, std::string value)
{
  globalHeader.put<std::string>(name, value);
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
