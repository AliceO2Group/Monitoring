///
/// \file ApMonBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ApMonBackend.h"
#include <iostream>
#include <sstream>
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

ApMonBackend::ApMonBackend(const std::string& configurationFile)
{
  try {
    mApMon = std::make_unique<ApMon>(const_cast<char*>(configurationFile.c_str()));
    ApMon::setLogLevel("FATAL");
  } 
  catch (...) {
    MonInfoLogger::Warning() << "Could not open ApMon configuration file: " << configurationFile
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    return;
  }
  MonInfoLogger::Info() << "ApMon backend initialized" << AliceO2::InfoLogger::InfoLogger::endm;
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
  
    case MetricType::UINT32_T :
    {  
      uint32_t value = boost::get<uint32_t>(metric.getValue());
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
        const_cast<char*>(metric.getName().c_str()), XDR_INT32, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    }
  }
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
