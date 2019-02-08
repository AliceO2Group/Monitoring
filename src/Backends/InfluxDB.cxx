///
/// \file InfluxDB.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include "../Transports/UDP.h"
#include "../Transports/HTTP.h"
#include "../Transports/Unix.h"
#include "../Exceptions/MonitoringException.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

InfluxDB::InfluxDB(const std::string& host, unsigned int port) :
  mTransport(std::make_unique<transports::UDP>(host, port))
{
  MonLogger::Get() << "InfluxDB/UDP backend initialized"
                   << " ("<< host << ":" << port << ")" << MonLogger::End();
}

InfluxDB::InfluxDB(const std::string& host, unsigned int port, const std::string& search)
{
  mTransport = std::make_unique<transports::HTTP>(
    "http://" + host + ":" + std::to_string(port) + "/write?" + search
  );
  MonLogger::Get() << "InfluxDB/HTTP backend initialized" << " (" << "http://" << host
                   << ":" <<  std::to_string(port) << "/write?" << search << ")" << MonLogger::End();
}

InfluxDB::InfluxDB(const std::string& socketPath) :
  mTransport(std::make_unique<transports::Unix>(socketPath))
{
  MonLogger::Get() << "InfluxDB/Unix backend initialized (" << socketPath << ")" << MonLogger::End();
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
    mTransport->send(convert.str());
  } catch (MonitoringException&) {
  }
}

void InfluxDB::send(std::vector<Metric>&& metrics) {
  std::string influxMetrics = "";
  for (const auto& metric : metrics) {
    influxMetrics += toInfluxLineProtocol(metric);
    influxMetrics += "\n";
  }

  try {
    mTransport->send(std::move(influxMetrics));
  } catch (MonitoringException&) {
  }

}

void InfluxDB::send(const Metric& metric)
{
  try {
    mTransport->send(toInfluxLineProtocol(metric));
  } catch (MonitoringException&) {
  }
}

std::string InfluxDB::toInfluxLineProtocol(const Metric& metric) {
  std::stringstream convert;
  std::string name = metric.getName();
  escape(name);
  convert << name << "," << tagSet;

  for (const auto& [key, value] : metric.getTags()) {
    convert << "," << tags::TAG_KEY[key] << "=" << tags::GetValue(value);
  }

  std::string value = boost::lexical_cast<std::string>(metric.getValue());
  prepareValue(value, metric.getType());

  convert << " value=" << value << " " << convertTimestamp(metric.getTimestamp());
  return convert.str();
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

void InfluxDB::addGlobalTag(std::string_view name, std::string_view value)
{
  std::string sName = name.data();
  std::string sValue = value.data();
  escape(sName); escape(sValue);
  if (!tagSet.empty()) tagSet += ",";
  tagSet += sName + "=" + sValue;
}

} // namespace backends
} // namespace monitoring
} // namespace o2
