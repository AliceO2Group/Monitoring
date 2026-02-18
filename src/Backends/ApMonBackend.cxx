// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
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
#include <unistd.h>
#include <limits.h>
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

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

ApMonBackend::ApMonBackend(const std::string& path)
{
  try {
    mApMon = std::make_unique<ApMon>(const_cast<char*>(path.c_str()));
    MonLogger::Get(Severity::Info) << "ApMon backend initialized" << MonLogger::End();
  } catch (std::runtime_error& e) {
    throw MonitoringException("ApMonBackend initialization", std::string(e.what()));
  }
}

inline int ApMonBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return static_cast<int>(std::chrono::system_clock::to_time_t(timestamp));
}

void ApMonBackend::addGlobalTag(std::string_view /*name*/, std::string_view value)
{
  if (!mEntity.empty()) {
    mEntity += ",";
  }
  mEntity += value;
}

std::string ApMonBackend::getNodeName()
{
  const char* env_p = std::getenv("ALIEN_PROC_ID");
  if (env_p) {
    return std::string(env_p);
  }
  
  char hostname[HOST_NAME_MAX];
  if (gethostname(hostname, sizeof(hostname)) == 0) {
    hostname[sizeof(hostname) - 1] = '\0';
    return std::string(hostname);
  }

  MonLogger::Get(Severity::Error) << "Failed to get hostname, using 'unknown'" << MonLogger::End();
  return "unknown";
}

void ApMonBackend::send(const Metric& metric)
{
  std::string clusterName(mClusterName);
  std::string metricName = metric.getName();
  std::string nodeName = getNodeName();
  std::string entity = mEntity;
  for (const auto& [key, value] : metric.getTags()) {
    entity += ',';
    entity += tags::TAG_KEY[key];
    entity += '=';
    (value > 0) ? entity += tags::GetValue(value) : entity += std::to_string(0 - value);
  }
  if (mRunNumber != 0) entity += (",run=" + std::to_string(mRunNumber));

  int valueSize = metric.getValuesSize();
  int totalParams = valueSize * 2; // each metric value has a source parameter
  char **paramNames, **paramValues;
  int* valueTypes;
  paramNames = (char**)std::malloc(totalParams * sizeof(char*));
  paramValues = (char**)std::malloc(totalParams * sizeof(char*));
  valueTypes = (int*)std::malloc(totalParams * sizeof(int));
  // the scope of values must be the same as sendTimedParameters method
  int intValue;
  double doubleValue;
  std::string stringValue;

  auto& values = metric.getValues();
  std::string sourceName = metricName + "_src";

  for (int i = 0; i < valueSize; ++i) {
    int metricIdx = i * 2;
    int sourceIdx = metricIdx + 1;
    paramNames[metricIdx] = const_cast<char*>(metricName.c_str());
    std::visit(overloaded{
      [&](int value) {
        valueTypes[metricIdx] = XDR_INT32;
        intValue = value;
        paramValues[metricIdx] = reinterpret_cast<char*>(&intValue);
      },
      [&](double value) {
        valueTypes[metricIdx] = XDR_REAL64;
        doubleValue = value;
        paramValues[metricIdx] = reinterpret_cast<char*>(&doubleValue);
      },
      [&](const std::string& value) {
        valueTypes[metricIdx] = XDR_STRING;
        stringValue = value;
        paramValues[metricIdx] = const_cast<char*>(stringValue.c_str());
      },
      [&](uint64_t value) {
        valueTypes[metricIdx] = XDR_REAL64;
        doubleValue = static_cast<double>(value);
        paramValues[metricIdx] = reinterpret_cast<char*>(&doubleValue);
      },
    }, values[metricIdx].second);
    
    paramNames[sourceIdx] = const_cast<char*>(sourceName.c_str());
    valueTypes[sourceIdx] = XDR_STRING;
    stringValue = entity;
    paramValues[sourceIdx] = const_cast<char*>(stringValue.c_str());
  }

  mApMon->sendTimedParameters(
    const_cast<char*>(clusterName.c_str()),
    const_cast<char*>(nodeName.c_str()),
    totalParams, paramNames, valueTypes, paramValues, 
    convertTimestamp(metric.getTimestamp())
  );

  std::free(paramNames);
  std::free(paramValues);
  std::free(valueTypes);
}

void ApMonBackend::send(std::vector<Metric>&& metrics)
{
  for (auto& metric : metrics) {
    send(metric);
  }
}

} // namespace backends
} // namespace monitoring
} // namespace o2
