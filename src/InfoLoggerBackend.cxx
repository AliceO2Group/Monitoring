///
/// \file InfoLoggerBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfoLoggerBackend.h"

#include <iostream>
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

inline unsigned long InfoLoggerBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

InfoLoggerBackend::InfoLoggerBackend()
{
  MonInfoLogger::Info() << "InfoLogger backend initialized" << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfoLoggerBackend::addGlobalTag(std::string name, std::string value)
{
  if (!tagString.empty()) {
    tagString += " ";
  }
  tagString += name + "=" + value;
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
  MonInfoLogger::Debug() << "InfoLoggerMonitoring : " << metric.getName() << ", " << metric.getValue() << " Type: " << metric.getType() << ", " 
                               << convertTimestamp(metric.getTimestamp()) << ", " << tagString << " " << metricTags
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

} // namespace Monitoring
} // namespace AliceO2
