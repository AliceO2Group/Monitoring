///
/// \file Flume.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Flume.h"
#include <string>
#include "../Transports/UDP.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

Flume::Flume(const std::string& host, unsigned int port)
{
  mTransport = std::make_unique<transports::UDP>(host, port);
  MonLogger::Get() << "Flume/UDP backend initialized"
                   << " ("<< host << ":" << port << ")" << MonLogger::End();
}

std::string Flume::metricToJson(const Metric& metric)
{
    boost::property_tree::ptree event;
    boost::property_tree::ptree header = globalHeader;
    header.put<std::string>("timestamp", std::to_string(convertTimestamp(metric.getTimestamp())));
    header.put<std::string>("name", metric.getName());
    header.put<std::string>("value_value", boost::lexical_cast<std::string>(metric.getValue()));

    for (const auto& tagIndex : metric.getTags()) {
      header.put<std::string>("tag_" + std::string(tags::TAG_ARRAY[tagIndex].first.data()), tags::TAG_ARRAY[tagIndex].second.data());
    }
    event.push_back(std::make_pair("headers", header));
    event.put<std::string>("body", "");
    std::stringstream ss; 
    write_json(ss, event);
    std::string s = ss.str();
    s.erase(std::remove_if( s.begin(), s.end(), 
      [](char c){ return (c =='\r' || c =='\t' || c == ' ' || c == '\n');}), s.end() );
    return s;
}

void Flume::send(std::vector<Metric>&& metrics)
{
  std::string flumeMetrics = "";
  for (auto& metric : metrics) {
    flumeMetrics += metricToJson(metric);
    flumeMetrics += '\n';
  }
  mTransport->send(std::move(flumeMetrics));
}

void Flume::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  mTransport->send(metricsToJson(measurement, std::move(metrics)) + '\n');
}

std::string Flume::metricsToJson(std::string measurement, std::vector<Metric>&& metrics)
{
  boost::property_tree::ptree event;
  boost::property_tree::ptree header = globalHeader;
  header.put<std::string>("timestamp", std::to_string(convertTimestamp(metrics.front().getTimestamp())));
  header.put<std::string>("name", measurement);
  for (const auto& tagIndex : metrics.front().getTags()) {
    header.put<std::string>("tag_" + std::string(tags::TAG_ARRAY[tagIndex].first.data()), tags::TAG_ARRAY[tagIndex].second.data());
  }
  for (auto& metric : metrics) {
    header.put<std::string>("value_" + metric.getName(), boost::lexical_cast<std::string>(metric.getValue()));
  }
  event.push_back(std::make_pair("headers", header));
    event.put<std::string>("body", "");
    std::stringstream ss;
    write_json(ss, event);
    std::string s = ss.str();
    s.erase(std::remove_if( s.begin(), s.end(),
      [](char c){ return (c =='\r' || c =='\t' || c == ' ' || c == '\n');}), s.end() );
    return s;
}

void Flume::send(const Metric& metric)
{
    std::string toSend = metricToJson(metric);
    toSend += '\n';
    mTransport->send(std::move(toSend));
}

inline unsigned long Flume::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void Flume::addGlobalTag(std::string_view name, std::string_view value)
{
  globalHeader.put<std::string>(name.data(), value.data());
}

} // namespace backends
} // namespace monitoring
} // namespace o2
