// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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

void ApMonBackend::addGlobalTag(std::string_view /*name*/, std::string_view value)
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
    switch(metrics[i].getType()) {
      case MetricType::INT :
      {
        valueTypes[i] = XDR_INT32;
        intValue = boost::get<int>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&intValue);
      }
      break;

      case MetricType::STRING :
      {
        valueTypes[i] = XDR_STRING;
        stringValue = boost::get<std::string>(metrics[i].getValue());
        paramValues[i] = const_cast<char*>(stringValue.c_str());
      }
      break;

      case MetricType::DOUBLE :
      {
        valueTypes[i] = XDR_REAL64;
        doubleValue = boost::get<double>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&doubleValue);
      }
      break;
    }
  }

  mApMon->sendTimedParameters(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
    noMetrics, paramNames, valueTypes, paramValues, convertTimestamp(metrics[0].getTimestamp()));

  std::free(paramNames);
  std::free(paramValues);
  std::free(valueTypes); 
}

void ApMonBackend::sendMultiple(std::string, std::vector<Metric>&& metrics)
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
    switch(metrics[i].getType()) {
      case MetricType::INT :
      {
        valueTypes[i] = XDR_INT32;
        intValue = boost::get<int>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&intValue);
      }
      break;

      case MetricType::STRING :
      {
        valueTypes[i] = XDR_STRING;
        stringValue = boost::get<std::string>(metrics[i].getValue());
        paramValues[i] = const_cast<char*>(stringValue.c_str());
      }
      break;

      case MetricType::DOUBLE :
      {
        valueTypes[i] = XDR_REAL64;
        doubleValue = boost::get<double>(metrics[i].getValue());
        paramValues[i] = reinterpret_cast<char*>(&doubleValue);
      }
      break;
    }
  }

  mApMon->sendTimedParameters(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
    noMetrics, paramNames, valueTypes, paramValues, convertTimestamp(metrics[0].getTimestamp()));

  std::free(paramNames);
  std::free(paramValues);
  std::free(valueTypes);
}

void ApMonBackend::send(const Metric& metric)
{
  std::string name = metric.getName();
  for (const auto& [key, value] : metric.getTags()) {
    name += ',';
    name += tags::TAG_KEY[key];
    name += "=";
    name += tags::GetValue(value);
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

} // namespace backends
} // namespace monitoring
} // namespace o2
