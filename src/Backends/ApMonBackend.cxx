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
#include <vector>
#include <map>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>
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

void ApMonBackend::sendBatch(const std::vector<reference_wrapper<const Metric>>& metrics)
{
  std::string clusterName(mClusterName);
  std::string nodeName = getNodeName();

  int totalValues = 0;
  for (const auto& metric : metrics) {
    totalValues += metric.get().getValuesSize();
  }
  const int totalParams = totalValues * 2;
  std::vector<int> intValues;
  std::vector<double> doubleValues;
  std::vector<std::string> stringValues;
  std::vector<char*> paramNames;
  std::vector<char*> paramValues;
  std::vector<int> valueTypes;

  intValues.reserve(totalValues);
  doubleValues.reserve(totalValues);
  stringValues.reserve(metrics.size() * 3 + totalValues);
  paramNames.reserve(totalParams);
  paramValues.reserve(totalParams);
  valueTypes.reserve(totalParams);

  for (const auto& metric : metrics) {
    std::string entity = mEntity;
    for (const auto& [key, value] : metric.get().getTags()) {
      entity += ',';
      entity += tags::TAG_KEY[key];
      entity += '=';
      (value > 0) ? entity += tags::GetValue(value) : entity += std::to_string(0 - value);
    }
    if (mRunNumber != 0) entity += (",run=" + std::to_string(mRunNumber));

    auto& values = metric.get().getValues();
    const int valueSize = metric.get().getValuesSize();

    const std::string_view metricName = metric.get().getName();
    stringValues.emplace_back(metricName);
    const char* metriNamePtr = stringValues.back().c_str();
    stringValues.emplace_back(std::string(metricName) + "_src");
    const char* metriNameSrcPtr = stringValues.back().c_str();
    stringValues.push_back(std::move(entity));
    const char* entityPtr = stringValues.back().c_str();

    for (int i = 0; i < valueSize; ++i) {
      paramNames.push_back(const_cast<char*>(metriNamePtr));
      std::visit(overloaded{
        [&](int value) {
          valueTypes.push_back(XDR_INT32);
          intValues.push_back(value);
          paramValues.push_back(reinterpret_cast<char*>(&intValues.back()));
        },
        [&](double value) {
          valueTypes.push_back(XDR_REAL64);
          doubleValues.push_back(value);
          paramValues.push_back(reinterpret_cast<char*>(&doubleValues.back()));
        },
        [&](const std::string& value) {
          valueTypes.push_back(XDR_STRING);
          stringValues.push_back(value);
          paramValues.push_back(const_cast<char*>(stringValues.back().c_str()));
        },
        [&](uint64_t value) {
          valueTypes.push_back(XDR_REAL64);
          doubleValues.push_back(static_cast<double>(value));
          paramValues.push_back(reinterpret_cast<char*>(&doubleValues.back()));
        },
      }, values[i].second);

      paramNames.push_back(const_cast<char*>(metriNameSrcPtr));
      valueTypes.push_back(XDR_STRING);
      paramValues.push_back(const_cast<char*>(entityPtr));
    }
  }

  mApMon->sendTimedParameters(
    const_cast<char*>(clusterName.c_str()),
    const_cast<char*>(nodeName.c_str()),
    totalParams, paramNames.data(), valueTypes.data(), paramValues.data(),
    convertTimestamp(metrics[0].get().getTimestamp())
  );
}

void ApMonBackend::send(const Metric& metric)
{
  sendBatch(std::vector<std::reference_wrapper<const Metric>>{std::cref(metric)});
}

void ApMonBackend::send(std::vector<Metric>&& metrics)
{
  if (metrics.empty()) {
    return;
  }

  std::map<int, std::vector<std::reference_wrapper<const Metric>>> metricsByTimestamp;
  for (const auto& metric : metrics) {
    metricsByTimestamp[convertTimestamp(metric.getTimestamp())].push_back(std::cref(metric));
  }

  for (const auto& [timestamp, metricsGroup] : metricsByTimestamp) {
    sendBatch(metricsGroup);
  }
}

} // namespace backends
} // namespace monitoring
} // namespace o2
