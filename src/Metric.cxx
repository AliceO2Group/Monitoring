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

int Metric::getType() const
{
  return mValue.which();
}

const std::string& Metric::getName() const
{
  return mName;
}

Metric::Metric(int value, const std::string& name) :
  mValue(value), mName(name), mTimestamp(Metric::getCurrentTimestamp())
{
}

Metric::Metric(std::string value, const std::string& name) :
  mValue(value), mName(name), mTimestamp(Metric::getCurrentTimestamp())
{}

Metric::Metric(double value, const std::string& name) :
  mValue(value), mName(name), mTimestamp(Metric::getCurrentTimestamp())
{}

Metric::Metric(uint64_t value, const std::string& name) :
  mValue(value), mName(name), mTimestamp(Metric::getCurrentTimestamp())
{}

Metric::Metric(boost::variant< int, std::string, double, uint64_t > value, const std::string& name) :
  mValue(value), mName(name), mTimestamp(Metric::getCurrentTimestamp())
{}

boost::variant< int, std::string, double, uint64_t > Metric::getValue() const
{
  return mValue;
}

Metric&& Metric::addTags(std::vector<unsigned int>&& tags)
{
  mTags = std::move(tags);
  return std::move(*this);
}

const std::vector<unsigned int>& Metric::getTags() const
{
  return mTags;
}

auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

} // namespace monitoring
} // namespace o2
