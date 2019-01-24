///
/// \file StdOut.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "StdOut.h"
#include "../MonLogger.h"
#include <iostream>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

inline unsigned long StdOut::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

StdOut::StdOut() : mStream(std::cout)
{
  setVerbosisty(Verbosity::DEBUG);
  MonLogger::Get() << "StdOut backend initialized" << MonLogger::End();
}

void StdOut::addGlobalTag(std::string_view name, std::string_view value)
{
  std::string tag = name.data();
  tag += "=";
  tag += value;
  if (!tagString.empty()) tagString += ",";
  tagString += tag;
}

void StdOut::send(std::vector<Metric>&& metrics) {
  for (auto& m : metrics) {
    send(m);
  }
}

void StdOut::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  std::string metricTags{};
  for (const auto& tagIndex : metrics.front().getTags()) {
    metricTags += ',';
    metricTags += tags::TAG_ARRAY[tagIndex].first;
    metricTags += "=";
    metricTags += tags::TAG_ARRAY[tagIndex].second;
  } 
  for (auto& metric : metrics) {
    mStream << "[METRIC] " << measurement << '/' << metric.getName() << ',' << metric.getType() << ' '
      << metric.getValue() << ' ' << convertTimestamp(metric.getTimestamp()) << ' ' << tagString
      << metricTags << '\n';
  }
}

void StdOut::send(const Metric& metric)
{
  mStream << "[METRIC] " << metric.getName() << ',' << metric.getType() << " " << metric.getValue()
          << ' ' << convertTimestamp(metric.getTimestamp()) << ' ' << tagString;

  for (const auto& tagIndex : metric.getTags()) {
    mStream << ',' << tags::TAG_ARRAY[tagIndex].first << "=" << tags::TAG_ARRAY[tagIndex].second;
  }
  mStream << '\n';
}

} // namespace backends
} // namespace monitoring
} // namespace o2
