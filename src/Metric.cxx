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
/// \file Metric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/Metric.h"

#include <iostream>
#include <chrono>
#include <memory>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

std::chrono::time_point<std::chrono::system_clock> Metric::getTimestamp() const
{
  return mTimestamp;
}

/*int Metric::getType() const
{
  if (std::holds_alternative<int>(mValue))
    return 0;
  else if (std::holds_alternative<std::string>(mValue))
    return 1;
  else if (std::holds_alternative<double>(mValue))
    return 2;
  else
    return 3;
}*/

const std::string& Metric::getName() const
{
  return mName;
}

Metric::Metric(int value, const std::string& name, Verbosity verbosity) : mName(name), mTimestamp(Metric::getCurrentTimestamp()), mVerbosity(verbosity)
{
  overwriteVerbosity();
  addValue("value", value);
}

Metric::Metric(std::string value, const std::string& name, Verbosity verbosity) : mName(name), mTimestamp(Metric::getCurrentTimestamp()), mVerbosity(verbosity)
{
  overwriteVerbosity();
  addValue("value", value);
}

Metric::Metric(double value, const std::string& name, Verbosity verbosity) : mName(name), mTimestamp(Metric::getCurrentTimestamp()), mVerbosity(verbosity)
{
  overwriteVerbosity();
  addValue("value", value);
}

Metric::Metric(uint64_t value, const std::string& name, Verbosity verbosity) : mName(name), mTimestamp(Metric::getCurrentTimestamp()), mVerbosity(verbosity)
{
  overwriteVerbosity();
  addValue("value", value);
}

Metric::Metric(const std::string& name, Verbosity verbosity) : mName(name), mTimestamp(Metric::getCurrentTimestamp()), mVerbosity(verbosity)
{
  overwriteVerbosity();
}

Verbosity Metric::getVerbosity()
{
  return mVerbosity;
}

void Metric::setVerbosityPolicy(Verbosity verbosity, const std::regex& regex)
{
  mRegexPolicy.insert({static_cast<std::underlying_type<tags::Value>::type>(verbosity), regex});
}

void Metric::overwriteVerbosity()
{
  for (auto const& [verbosity, regex] : mRegexPolicy) {
    if (std::regex_match(mName, regex)) {
      mVerbosity = static_cast<Verbosity>(verbosity);
    }
  }
}

Metric&& Metric::addTag(tags::Key key, tags::Value value)
{
  mTags.push_back({static_cast<std::underlying_type<tags::Key>::type>(key), static_cast<std::underlying_type<tags::Value>::type>(value)});
  return std::move(*this);
}

Metric&& Metric::addTag(tags::Key key, unsigned short number)
{
  mTags.push_back({static_cast<std::underlying_type<tags::Key>::type>(key), 0 - number});
  return std::move(*this);
}

Metric&& Metric::setTags(std::vector<std::pair<int, int>>&& tags)
{
  mTags = std::move(tags);
  return std::move(*this);
}

const std::vector<std::pair<int, int>>& Metric::getTags() const
{
  return mTags;
}

auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

void Metric::setDefaultVerbosity(Verbosity verbosity)
{
  Metric::DefaultVerbosity = verbosity;
}

Metric&& Metric::addValue(const std::string& name, int value)
{
  mValues.push_back({name, value});
  return std::move(*this);
}
Metric&& Metric::addValue(const std::string& name, double value)
{
  mValues.push_back({name, value});
  return std::move(*this);
}
Metric&& Metric::addValue(const std::string& name, uint64_t value)
{
  mValues.push_back({name, value});
  return std::move(*this);
}

Metric&& Metric::addValue(const std::string& name, std::string value)
{
  mValues.push_back({name, value});
  return std::move(*this);
}

Metric&& Metric::addValue(const std::string& name, const std::variant<int, std::string, double, uint64_t>& value)
{
  mValues.push_back({name, value});
  return std::move(*this);
}

const std::vector<std::pair<std::string, std::variant<int, std::string, double, uint64_t>>>& Metric::getValues() const
{
  return mValues;
}

const std::pair<std::string, std::variant<int, std::string, double, uint64_t>>& Metric::getFirstValue() const
{
  return mValues.front();
}

std::size_t Metric::getValuesSize() const noexcept
{
  return mValues.size();
}

bool Metric::includeTimestamp = true;
Verbosity Metric::DefaultVerbosity = Verbosity::Info;
std::map<std::underlying_type<Verbosity>::type, std::regex> Metric::mRegexPolicy;

} // namespace monitoring
} // namespace o2
