///
/// \file InfluxDB.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include "../Transports/UDP.h"
#include "../Transports/HTTP.h"
#include "../Exceptions/MonitoringInternalException.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

InfluxDB::InfluxDB(const std::string &hostname, int port)
{ 
  transport = std::make_unique<Transports::UDP>(hostname, port);
  MonLogger::Get() << "InfluxDB/UDP backend initialized"
                       << " ("<< hostname << ":" << port << ")" << MonLogger::End();
}

InfluxDB::InfluxDB(const std::string &hostname, int port, const std::string& database)
{
  transport = std::make_unique<Transports::HTTP>(
    "http://" + hostname + ":" + std::to_string(port) + "/write?db=" + database
  );
  MonLogger::Get() << "InfluxDB/HTTP backend initialized" << " ("<< hostname 
                       << ":" << port << "/write?db=" << database << ")" << MonLogger::End();
}

inline unsigned long InfluxDB::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void InfluxDB::escape(std::string& escaped)
{
  boost::replace_all(escaped, ",", "\\,");
  boost::replace_all(escaped, "=", "\\=");
  boost::replace_all(escaped, " ", "\\ ");
}

void InfluxDB::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  escape(measurement);
  std::stringstream convert;
  convert << measurement << "," << tagSet << " ";

  for (const auto& metric : metrics) {
    std::string value = boost::lexical_cast<std::string>(metric.getValue());
    prepareValue(value, metric.getType());
    convert << metric.getName() << "=" << value << ",";
  }
  convert.seekp(-1, std::ios_base::end);
  convert << " " <<  convertTimestamp(metrics.back().getTimestamp());

  try {
    transport->send(convert.str());
  } catch (MonitoringInternalException&) {
  }
}

void InfluxDB::send(const Metric& metric)
{
  std::string metricTags{};
  for (const auto& tag : metric.getTags()) {
    metricTags += "," + tag.name + "=" + tag.value;
  }

  std::string value = boost::lexical_cast<std::string>(metric.getValue());
  prepareValue(value, metric.getType());
  std::string name = metric.getName();
  escape(name);

  std::stringstream convert;
  convert << name << "," << tagSet << metricTags << " value=" << value << " " << convertTimestamp(metric.getTimestamp());

  try {
    transport->send(convert.str());
  } catch (MonitoringInternalException&) {
  }
}

void InfluxDB::prepareValue(std::string& value, int type)
{
  if (type == MetricType::STRING) {
    escape(value);
    value.insert(value.begin(), '"');
    value.insert(value.end(), '"');
  }

  if (type == MetricType::INT) {
    value.insert(value.end(), 'i');
  }
}

void InfluxDB::addGlobalTag(std::string name, std::string value)
{
  escape(name); escape(value);
  if (!tagSet.empty()) tagSet += ",";
  tagSet += name + "=" + value;
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
