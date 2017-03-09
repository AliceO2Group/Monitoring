///
/// \file ApMonBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ApMonBackend.h"
#include <iostream>
#include <sstream>
#include "../MonInfoLogger.h"
#include "../Exceptions/MonitoringException.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Monitoring backends
namespace Backends
{

using AliceO2::InfoLogger::InfoLogger;

ApMonBackend::ApMonBackend(const std::string& configurationFile)
{
  try {
    mApMon = std::make_unique<ApMon>(const_cast<char*>(configurationFile.c_str()));
    MonInfoLogger::Get() << "ApMon backend initialized" << InfoLogger::endm;
  } 
  catch (std::runtime_error &e) {
    throw MonitoringException("ApMonBackend initialization", std::string(e.what()) + " (" + configurationFile + ")");
  }
}

inline int ApMonBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void ApMonBackend::addGlobalTag(std::string name, std::string value)
{
  if (!entity.empty()) entity += ".";
  entity += value;
}

void ApMonBackend::send(const Metric& metric)
{
  std::string name = metric.getName();
  for (const auto& tag : metric.getTags()) {
    name += "," + tag.name + "=" + tag.value;
  } 
 
  switch(metric.getType()) {
    case MetricType::INT :
    {
      int value = boost::get<int>(metric.getValue());
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
        const_cast<char*>(metric.getName().c_str()), XDR_INT32, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    }
    break;
    
    case MetricType::STRING :
    {
      std::string value = boost::get<std::string>(metric.getValue());
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
        const_cast<char*>(metric.getName().c_str()), XDR_STRING, const_cast<char*>(value.c_str()), convertTimestamp(metric.getTimestamp()));
    }
    break;

    case MetricType::DOUBLE :
    {
      double value = boost::get<double>(metric.getValue());
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()), 
        const_cast<char*>(metric.getName().c_str()), XDR_REAL64, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    }
    break;
  }
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
