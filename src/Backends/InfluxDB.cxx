///
/// \file InfluxDB.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include <string>
#include "../Transports/UDP.h"
#include "../Transports/HTTP.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

using AliceO2::InfoLogger::InfoLogger;

InfluxDB::InfluxDB(const std::string &hostname, int port)
{ 
  try {
    transport = std::make_unique<Transports::UDP>(hostname, port);
    MonInfoLogger::Get() << "InfluxDB/UDP backend initialized"
                         << " ("<< hostname << ":" << port << ")" << InfoLogger::endm;
  } catch(...) {
    MonInfoLogger::Get() << InfoLogger::Severity::Error << "Failed to initialize InfluxDB/UDP backend"
                         << " ("<< hostname << ":" << port << ")" << InfoLogger::endm;
  }
}

InfluxDB::InfluxDB(const std::string &hostname, int port, const std::string& database)
{
  transport = std::make_unique<Transports::HTTP>(hostname, port, database);
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

void InfluxDB::send(const Metric& metric)
{
  std::string value = boost::lexical_cast<std::string>(metric.getValue());
  if (metric.getType() == MetricType::STRING) {
    escape(value);
    value.insert(value.begin(), '"');
    value.insert(value.end(), '"');
  }
  std::string name = metric.getName();
  escape(name);

  std::stringstream convert;
  convert << name << "," << tagSet << " value=" << value << " " << convertTimestamp(metric.getTimestamp());
  
  try {
    transport->send(convert.str());
  } catch (std::runtime_error& e) {
    MonInfoLogger::Get() << InfoLogger::Severity::Warning << "Monitoring: " << e.what() << InfoLogger::endm;
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
