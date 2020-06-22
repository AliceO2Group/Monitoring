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

StdOut::StdOut(const std::string& prefix) : mStream(std::cout), mPrefix(prefix)
{
  setVerbosisty(Verbosity::Debug);
  MonLogger::Get() << "StdOut backend initialized" << MonLogger::End();
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
  mStream << "[" << mPrefix << "] " << metric.getName();
  for (auto& value : metric.getValues()) {
    auto stringValue = std::visit(overloaded{
                                    [](const std::string& value) -> std::string { return value; },
                                    [](auto value) -> std::string { return std::to_string(value); }},
                                  value.second);
    if (metric.getValuesSize() == 1) {
      mStream << ',' << metric.getFirstValueType() << ' ' << stringValue;
    } else {
      mStream << ' ' << value.first << '=' << stringValue;
    }
  }
  mStream << ' ' << convertTimestamp(metric.getTimestamp()) << ' ' << tagString;

  for (const auto& [key, value] : metric.getTags()) {
    mStream << ',' << tags::TAG_KEY[key] << "=" << tags::GetValue(value);
  }
  mStream << '\n';
}

} // namespace backends
} // namespace monitoring
} // namespace o2
