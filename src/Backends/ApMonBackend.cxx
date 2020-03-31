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
    MonLogger::Get() << "ApMon backend initialized" << MonLogger::End();
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

void ApMonBackend::send(const Metric& metric)
{
  std::string name = metric.getName();
  std::string entity = mEntity;
  for (const auto& [key, value] : metric.getTags()) {
    entity += ',';
    entity += tags::GetValue(value);
  }

  int valueSize = metric.getValuesSize();
  char **paramNames, **paramValues;
  int* valueTypes;
  paramNames = (char**)std::malloc(valueSize * sizeof(char*));
  paramValues = (char**)std::malloc(valueSize * sizeof(char*));
  valueTypes = (int*)std::malloc(valueSize * sizeof(int));
  // the scope of values must be the same as sendTimedParameters method
  int intValue;
  double doubleValue;
  std::string stringValue;

  auto& values = metric.getValues();
      
  for (int i = 0; i < valueSize; i++) {
    paramNames[i] = const_cast<char*>(values[i].first.c_str());
    std::visit(overloaded{
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
    }, values[i].second);
  }

  mApMon->sendTimedParameters(const_cast<char*>(name.c_str()), const_cast<char*>(entity.c_str()),
                              valueSize, paramNames, valueTypes, paramValues, convertTimestamp(metric.getTimestamp()));

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
