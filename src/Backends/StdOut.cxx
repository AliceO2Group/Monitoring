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

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

inline unsigned long StdOut::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           timestamp.time_since_epoch())
    .count();
}

StdOut::StdOut(const std::string& prefix) : mPrefix(prefix)
{
  setVerbosity(Verbosity::Debug);
}

void StdOut::addGlobalTag(std::string_view name, std::string_view value)
{
  std::string tag = name.data();
  tag += "=";
  tag += value;
  if (!tagString.empty())
    tagString += ",";
  tagString += tag;
}

void StdOut::send(std::vector<Metric>&& metrics)
{
  for (auto& m : metrics) {
    send(m);
  }
}

void StdOut::send(const Metric& metric)
{
  std::ostringstream convert;
  convert << "[" << mPrefix << "] "  << metric.getName();
  for (auto& value : metric.getValues()) {
    auto stringValue = std::visit(overloaded{
      [](const std::string& value) -> std::string { return value; },
      [](auto value) -> std::string { return std::to_string(value); }
    }, value.second);
    if (metric.getValuesSize() == 1) {
      convert << ',' << metric.getFirstValueType() << ' ' << stringValue;
    } else {
      convert << ' ' << value.first << '=' << stringValue;
    }
  }
  convert << ' ' << convertTimestamp(metric.getTimestamp()) << ' ' << tagString;

  for (const auto& [key, value] : metric.getTags()) {
    convert << ',' << tags::TAG_KEY[key] << "=";
    (value > 0) ? convert << tags::GetValue(value) : convert << (0 - value);
  }
  if (mRunNumber != 0) convert << ",run=" << mRunNumber;
  convert << '\n';
  std::cout << convert.str();
}

} // namespace backends
} // namespace monitoring
} // namespace o2
