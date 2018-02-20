///
/// \file InfoLoggerBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfoLoggerBackend.h"

#include <iostream>
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

inline unsigned long InfoLoggerBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

InfoLoggerBackend::InfoLoggerBackend()
{
  MonLogger::Get() << "Local InfoLogger backend initialized" << MonLogger::End();
}

void InfoLoggerBackend::addGlobalTag(std::string name, std::string value)
{
  if (!tagString.empty()) {
    tagString += " ";
  }
  tagString += name + "=" + value;
}

void InfoLoggerBackend::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  for (auto& m : metrics) {
    std::string tempName = m.getName();
    m.setName(measurement + "-" + m.getName());
    send(m);
    m.setName(tempName);
  }
}

void InfoLoggerBackend::send(const Metric& metric)
{
  std::string metricTags{};
  for (const auto& tag : metric.getTags()) {
    if (!metricTags.empty()) {
      metricTags += " ";
    }
    metricTags += tag.name + "=" + tag.value;
  }
  MonLogger::Get() << "InfoLoggerMonitoring : " << metric.getName() << ", "
    << metric.getValue() << " Type: " << metric.getType() << ", " 
    << convertTimestamp(metric.getTimestamp()) << ", " << tagString << " " << metricTags
    << MonLogger::End();
}

} // namespace backends
} // namespace monitoring
} // namespace o2
