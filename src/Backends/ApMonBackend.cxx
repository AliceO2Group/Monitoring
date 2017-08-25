///
/// \file ApMonBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ApMonBackend.h"
#include <iostream>
#include <sstream>
#include "../MonLogger.h"
#include "../Exceptions/MonitoringException.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Monitoring backends
namespace Backends
{

ApMonBackend::ApMonBackend(const std::string& configurationFile)
{
  try {
    mApMon = std::make_unique<ApMon>(const_cast<char*>(configurationFile.c_str()));
    MonLogger::Get() << "ApMon backend initialized" << MonLogger::End();
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

void ApMonBackend::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
  size_t noMetrics = metrics.size();
  char **paramNames, **paramValues;
  int *valueTypes;
  paramNames = (char **)malloc(noMetrics * sizeof(char *));
  paramValues = (char **)malloc(noMetrics * sizeof(char *));
  valueTypes = (int *)malloc(noMetrics * sizeof(int));

  for (int i = 0; i < noMetrics; i++) {
    paramNames[i] = const_cast<char*>(metrics[i].getName().c_str());
    switch(metrics[i].getType()) {
      case MetricType::INT :
      {
        valueTypes[i] = XDR_INT32;
        int value = boost::get<int>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&value);
      }
      break;

      case MetricType::STRING :
      {
        valueTypes[i] = XDR_STRING;
        std::string value = boost::get<std::string>(metrics[i].getValue());
        paramValues[i] = const_cast<char*>(value.c_str());
      }
      break;

      case MetricType::DOUBLE :
      {
        valueTypes[i] = XDR_REAL64;
        double value = boost::get<double>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&value);
      }
      break;
    }

    mApMon->sendTimedParameters(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
     noMetrics, paramNames, valueTypes, paramValues, convertTimestamp(metrics[0].getTimestamp()));
  }
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
