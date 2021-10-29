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
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "Monitoring/DerivedMetrics.h"
#include "Exceptions/MonitoringException.h"
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "VariantVisitorAdd.h"
#include "VariantVisitorRate.h"

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

bool DerivedMetrics::process(Metric& metric, DerivedMetricMode mode)
{
  const std::map<DerivedMetricMode, std::function<bool(Metric&)>> map = {
    {DerivedMetricMode::INCREMENT, [this](Metric& metric) {
       auto tags = metric.getTags();
       std::string key = metric.getName();
       std::for_each(tags.begin(), tags.end(), [&key](auto const& pair) {
         key += pair.second;
       });
       auto search = mStorage.find(key);
       if (search != mStorage.end()) {
         auto currentValue = metric.getFirstValue().second;
         auto storedValue = search->second.getValues().back().second;
         auto value = std::visit(VariantVisitorAdd{}, currentValue, storedValue);
         mStorage.erase(search);
         metric.addValue(value, metric.getFirstValue().first + "_increment");
       } else {
         metric.addValue(metric.getFirstValue().second, metric.getFirstValue().first + "_increment");
       }
       mStorage.insert(std::make_pair(key, metric));
       return true;
     }},
    {DerivedMetricMode::RATE, [this](Metric& metric) {
       /// create pseudo unique key
       auto tags = metric.getTags();
       std::string key = metric.getName();
       std::for_each(tags.begin(), tags.end(), [&key](auto const& pair) {
         key += pair.second;
       });

       // search for previous value
       auto search = mStorage.find(key);
       if (search == mStorage.end()) {
         mStorage.insert(std::make_pair(key, metric));
         metric.addValue((double)0.0, metric.getFirstValue().first + "_rate");
         return true;
       }

       auto timestampDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
         metric.getTimestamp() - search->second.getTimestamp());
       int timestampCount = timestampDifference.count();
       // disallow dividing by 0
       if (timestampCount == 0) {
         throw MonitoringException("DerivedMetrics", "Division by 0");
       }

       auto current = metric.getFirstValue().second;
       auto previous = search->second.getFirstValue().second;
       auto rate = std::visit(VariantVisitorRate(timestampCount), current, previous);

       // handle situation when a new run starts
       auto isZero = std::visit(overloaded{
         [](auto arg) { return arg == 0; },
         [](const std::string& arg) { return arg == ""; }
       }, current);
       if (rate < 0 && isZero) {
         rate = 0;
       }

       // swap metrics
       mStorage.erase(key);
       mStorage.insert(std::make_pair(key, metric));
       // add rate field
       metric.addValue(rate, metric.getFirstValue().first + "_rate");
       return true;
    }},
    {DerivedMetricMode::SUPPRESS, [this](Metric& metric) {
       auto tags = metric.getTags();
       std::string key = metric.getName();
       std::for_each(tags.begin(), tags.end(), [&key](auto const& pair) {
         key += pair.second;
       });

       // if no previous values -> store and send
       auto search = mStorage.find(key);
       if (search == mStorage.end()) {
         mStorage.insert(std::make_pair(key, metric));
         return true;
       }
       auto current = metric.getFirstValue().second;
       auto previous = search->second.getFirstValue().second;
       if (current == previous) {
         return false;
       }
       mStorage.erase(key);
       mStorage.insert(std::make_pair(key, metric));
       return true;
     }}
  };
  auto iterator = map.find(mode);
  if (iterator == map.end()) {
    throw MonitoringException("DerivedMetrics", "Unknown mode");
  }
  return iterator->second(metric);
}

} // namespace monitoring
} // namespace o2
