///
/// \file InfluxDB.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include <string>
#include <variant>
#include "../Transports/UDP.h"
#include "../Transports/HTTP.h"
#include "../Exceptions/MonitoringException.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

InfluxDB::InfluxDB(const std::string& host, unsigned int port)
{
  transport = std::make_unique<transports::UDP>(host, port);
  MonLogger::Get() << "InfluxDB/UDP backend initialized"
                   << " ("<< host << ":" << port << ")" << MonLogger::End();
}

InfluxDB::InfluxDB(const std::string& host, unsigned int port, const std::string& path, const std::string& search)
{
  transport = std::make_unique<transports::HTTP>(
    "http://" + host + ":" + std::to_string(port) + path + "?" + search
  );
  MonLogger::Get() << "InfluxDB/HTTP backend initialized" << " (" << "http://" << host
                   << ":" <<  std::to_string(port) << path << "?" << search << ")" << MonLogger::End();
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
   convert << metric.getName() << "=";
   std::visit(overloaded {
      [&convert](uint64_t value) { convert << value << 'i'; },
      [&convert](int value) { convert << value << 'i'; },
      [&convert](double value) { convert << value; },
      [&convert](const std::string& value) { convert << '"' << value << '"'; },
      }, metric.getValue());
    convert << ",";
  }
  convert.seekp(-1, std::ios_base::end);
  convert << " " <<  convertTimestamp(metrics.back().getTimestamp());

  try {
    transport->send(convert.str());
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
    transport->send(std::move(influxMetrics));
  } catch (MonitoringException&) {
  }

}

void InfluxDB::send(const Metric& metric)
{
  try {
    transport->send(toInfluxLineProtocol(metric));
  } catch (MonitoringException&) {
  }
}

std::string InfluxDB::toInfluxLineProtocol(const Metric& metric) {
  std::string metricTags{};
  for (const auto& tag : metric.getTags()) {
    metricTags += "," + tag.name + "=" + tag.value;
  }

  std::stringstream convert;
  std::string name = metric.getName();
  escape(name);
  convert << name << "," << tagSet << metricTags << " value=";

  std::visit(overloaded {
    [&convert](uint64_t value) { convert << value << 'i'; },
    [&convert](int value) { convert << value << 'i'; },
    [&convert](double value) { convert << value; },
    [&convert](const std::string& value) { convert << '"' << value << '"'; },
    }, metric.getValue());

  convert << " " << convertTimestamp(metric.getTimestamp());
  return convert.str();
}

void InfluxDB::addGlobalTag(std::string name, std::string value)
{
  escape(name); escape(value);
  if (!tagSet.empty()) tagSet += ",";
  tagSet += name + "=" + value;
}

} // namespace backends
} // namespace monitoring
} // namespace o2
