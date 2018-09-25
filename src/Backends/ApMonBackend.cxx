///
/// \file ApMonBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ApMonBackend.h"
#include <iostream>
#include <sstream>
#include "../MonLogger.h"
#include "../Exceptions/MonitoringException.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

ApMonBackend::ApMonBackend(const std::string& path)
{
  try {
    mApMon = std::make_unique<ApMon>(const_cast<char*>(path.c_str()));
    MonLogger::Get() << "ApMon backend initialized" << MonLogger::End();
  }
  catch (std::runtime_error &e) {
    throw MonitoringException("ApMonBackend initialization", std::string(e.what()));
  }
}

inline int ApMonBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return static_cast<int>(std::chrono::system_clock::to_time_t(timestamp));
}

void ApMonBackend::addGlobalTag(std::string, std::string value)
{
  if (!entity.empty()) entity += ".";
  entity += value;
}

void ApMonBackend::send(std::vector<Metric>&& metrics)
{
  int noMetrics = metrics.size();
  char **paramNames, **paramValues;
  int *valueTypes;
  paramNames = (char **)std::malloc(noMetrics * sizeof(char *));
  paramValues = (char **)std::malloc(noMetrics * sizeof(char *));
  valueTypes = (int *)std::malloc(noMetrics * sizeof(int));
  // the scope of values must be the same as sendTimedParameters method
  int intValue;
  double doubleValue;
  std::string stringValue;

  for (int i = 0; i < noMetrics; i++) {
    paramNames[i] = const_cast<char*>(metrics[i].getName().c_str());
    std::visit(overloaded {
      [&](int value) {
        valueTypes[i] = XDR_INT32;
        intValue = value;
        paramValues[i] = reinterpret_cast<char*>(&intValue);
      },
      [&](double value) {
        valueTypes[i] = XDR_REAL64;
        doubleValue = value;
        paramValues[i] = reinterpret_cast<char*>(&doubleValue);
      },
      [&](const std::string& value) {
        valueTypes[i] = XDR_STRING;
        stringValue = value;
        paramValues[i] = const_cast<char*>(stringValue.c_str());
      },
      [&](uint64_t value) {
        valueTypes[i] = XDR_REAL64;
        doubleValue = static_cast<double>(value);
        paramValues[i] = reinterpret_cast<char*>(&doubleValue);
      },
    }, metrics[i].getValue());
  }

  mApMon->sendTimedParameters(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
    noMetrics, paramNames, valueTypes, paramValues, convertTimestamp(metrics[0].getTimestamp()));

  std::free(paramNames);
  std::free(paramValues);
  std::free(valueTypes); 
}

void ApMonBackend::sendMultiple(std::string, std::vector<Metric>&& metrics)
{
  send(std::move(metrics));
}

void ApMonBackend::send(const Metric& metric)
{
  std::string name = metric.getName();
  for (const auto& tag : metric.getTags()) {
    name += "," + tag.name + "=" + tag.value;
  }

  std::visit(overloaded {
    [this, &metric](int value) {
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
        const_cast<char*>(metric.getName().c_str()), XDR_INT32, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    },
    [this, &metric](double value) {
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()), 
        const_cast<char*>(metric.getName().c_str()), XDR_REAL64, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    },
    [this, &metric](const std::string& value) {
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
        const_cast<char*>(metric.getName().c_str()), XDR_STRING, const_cast<char*>(value.c_str()), convertTimestamp(metric.getTimestamp()));
    },
    [this, &metric](uint64_t value) {
      mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()), 
        const_cast<char*>(metric.getName().c_str()), XDR_REAL64, reinterpret_cast<char*>(&value), convertTimestamp(metric.getTimestamp()));
    },
   }, metric.getValue());
}

} // namespace backends
} // namespace monitoring
} // namespace o2
