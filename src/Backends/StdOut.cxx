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
  setVerbosisty(backend::Verbosity::Debug);
  MonLogger::Get() << "StdOut backend initialized" << MonLogger::End();
}

void StdOut::addGlobalTag(std::string_view tag)
{
  if (!tagString.empty()) {
    tagString += ",";
  }
  tagString += tag;
}

void StdOut::send(std::vector<Metric>&& metrics) {
  for (auto& m : metrics) {
    send(m);
  }
}

void StdOut::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  for (auto& metric : metrics) {
    char delimeter = '\0';
    if (!metric.getTags().empty() && !tagString.empty()) {
      delimeter = ',';
    }
    mStream <<  "[METRIC] " << measurement << "/" << metric.getConstName() << "," << metric.getType() << " "
      << metric.getValue() << " " << convertTimestamp(metric.getTimestamp()) << " " << tagString << delimeter
      << metric.getTags() << "\n";
  }
}

void StdOut::send(const Metric& metric)
{
  char delimeter = '\0';
  if (!metric.getTags().empty() && !tagString.empty()) {
    delimeter = ',';
  }
  mStream << "[METRIC] " << metric.getConstName() << "," << metric.getType() << " " << metric.getValue()
    << " " << convertTimestamp(metric.getTimestamp()) << " " << tagString << delimeter << metric.getTags() << "\n";
}

} // namespace backends
} // namespace monitoring
} // namespace o2

