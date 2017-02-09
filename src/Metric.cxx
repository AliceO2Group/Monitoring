///
/// \file Metric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/Metric.h"

#include <iostream>
#include <chrono>
#include <memory>
#include <vector>


namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

std::chrono::time_point<std::chrono::system_clock> Metric::getTimestamp() const
{
  return mTimestamp;
}

int Metric::getType() const
{
  return mValue.which();
}

std::string Metric::getName() const
{
  return mName;
}

Metric::Metric(int value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

Metric::Metric(std::string value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

Metric::Metric(double value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

Metric::Metric(uint32_t value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

boost::variant< int, std::string, double, uint32_t > Metric::getValue() const
{
  return mValue;
}

void Metric::addTags(std::vector<Tag>&& tags)
{
  tagSet = std::move(tags);
}

auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
