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
#include <sstream>
#include <string>
#include <variant>
#include <boost/algorithm/string/replace.hpp>
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

InfluxDB::InfluxDB(std::unique_ptr<transports::TransportInterface> transport)
  : mTransport(std::move(transport))
{
  MonLogger::Get() << "InfluxDB backend initialized" << MonLogger::End();
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

void InfluxDB::send(std::vector<Metric>&& metrics)
{
  std::string influxMetrics = "";
  for (const auto& metric : metrics) {
    influxMetrics += toInfluxLineProtocol(metric);
    influxMetrics += "\n";
  }
  //remove last \n
  influxMetrics.pop_back();

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
  convert << ' ';
  for (const auto& [name, value] : metric.getValues()) {
    convert << name << '=';
    std::visit(overloaded{
               [&convert](uint64_t value) { convert << value << 'i'; },
               [&convert](int value) { convert << value << 'i'; },
               [&convert](double value) { convert << value; },
               [&convert](const std::string& value) { convert << '"' << value << '"'; },
             },  
             value);
    convert << ',';
  }
  convert.seekp(-1, std::ios_base::end);
  if (Metric::includeTimestamp) {
    convert << " " << convertTimestamp(metric.getTimestamp());
  }
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
