// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file InfluxDB.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include <string>
#include <variant>
#include "../Transports/UDP.h"
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

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

InfluxDB::InfluxDB(const std::string& host, unsigned int port) : mTransport(std::make_unique<transports::UDP>(host, port))
{
  MonLogger::Get() << "InfluxDB/UDP backend initialized"
                   << " (" << host << ":" << port << ")" << MonLogger::End();
}

InfluxDB::InfluxDB() {}

InfluxDB::InfluxDB(const std::string& socketPath) : mTransport(std::make_unique<transports::Unix>(socketPath))
{
  MonLogger::Get() << "InfluxDB/Unix backend initialized (" << socketPath << ")" << MonLogger::End();
}

inline unsigned long InfluxDB::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
           timestamp.time_since_epoch())
    .count();
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
    std::visit(overloaded{
                 [&convert](uint64_t value) { convert << value << 'i'; },
                 [&convert](int value) { convert << value << 'i'; },
                 [&convert](double value) { convert << value; },
                 [&convert](const std::string& value) { convert << '"' << value << '"'; },
               },
               metric.getValue());
    convert << ",";
  }
  convert.seekp(-1, std::ios_base::end);
  convert << " " << convertTimestamp(metrics.back().getTimestamp());

  try {
    mTransport->send(convert.str());
  } catch (MonitoringException&) {
  }
}

void InfluxDB::send(std::vector<Metric>&& metrics)
{
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

std::string InfluxDB::toInfluxLineProtocol(const Metric& metric)
{
  std::stringstream convert;
  std::string name = metric.getName();
  escape(name);
  convert << name << "," << tagSet;

  for (const auto& [key, value] : metric.getTags()) {
    convert << "," << tags::TAG_KEY[key] << "=" << tags::GetValue(value);
  }

  convert << " value=";

  std::visit(overloaded{
               [&convert](uint64_t value) { convert << value << 'i'; },
               [&convert](int value) { convert << value << 'i'; },
               [&convert](double value) { convert << value; },
               [&convert](const std::string& value) { convert << '"' << value << '"'; },
             },
             metric.getValue());

  convert << " " << convertTimestamp(metric.getTimestamp());
  return convert.str();
}

void InfluxDB::addGlobalTag(std::string_view name, std::string_view value)
{
  std::string sName = name.data();
  std::string sValue = value.data();
  escape(sName);
  escape(sValue);
  if (!tagSet.empty())
    tagSet += ",";
  tagSet += sName + "=" + sValue;
}

} // namespace backends
} // namespace monitoring
} // namespace o2
