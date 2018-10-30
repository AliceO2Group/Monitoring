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

/// This is required for backward compability with boost::variant
int Metric::getType() const
{
  if (std::holds_alternative<int>(mValue)) return 0;
  else if (std::holds_alternative<std::string>(mValue)) return 1;
  else if (std::holds_alternative<double>(mValue)) return 2;
  else return 3;
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

Metric::Metric(uint64_t value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

Metric::Metric(std::variant< int, std::string, double, uint64_t > value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mTimestamp(timestamp)
{}

Metric::Metric(const Metric& other)
{
  std::lock_guard<std::mutex> lock(other.mValueMutex);
  mName = other.mName;
  mValue = other.mValue;
  mTimestamp = other.mTimestamp;
  tagSet = other.tagSet;
}

Metric& Metric::operator=(Metric const& other)
{
  if (&other != this) {
    std::unique_lock<std::mutex> lockThis(mValueMutex, std::defer_lock);
    std::unique_lock<std::mutex> lockOther(other.mValueMutex, std::defer_lock);
    std::lock(lockThis, lockOther);

    mName = other.mName;
    mValue = other.mValue;
    mTimestamp = other.mTimestamp;
    tagSet = other.tagSet;
  }
  return *this;
}

std::variant< int, std::string, double, uint64_t > Metric::getValue() const
{
  return mValue;
}

Metric& Metric::operator=(const std::variant< int, std::string, double, uint64_t >& value) {
  mValue = value;
  return *this;
}

Metric&& Metric::addTags(std::vector<Tag>&& tags)
{
  tagSet = std::move(tags);
  return std::move(*this);
}

Metric&& Metric::setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  mTimestamp = timestamp;
  return std::move(*this);
}

std::vector<Tag> Metric::getTags() const
{
  return tagSet;
}

auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

} // namespace monitoring
} // namespace o2
