///
/// \file InfoLoggerBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfoLoggerBackend.h"

#include <iostream>
#include "../MonLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

using AliceO2::InfoLogger::InfoLogger;

inline unsigned long InfoLoggerBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

InfoLoggerBackend::InfoLoggerBackend()
{
  MonLogger::Get() << "InfoLogger backend initialized" << InfoLogger::endm;
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
  mInfoLogger << InfoLogger::Severity::Debug << "InfoLoggerMonitoring : " << metric.getName() << ", "
    << metric.getValue() << " Type: " << metric.getType() << ", " 
    << convertTimestamp(metric.getTimestamp()) << ", " << tagString << " " << metricTags
    << InfoLogger::endm;
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
