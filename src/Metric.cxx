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

std::string Metric::getEntity() const
{
  return mEntity;
}

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

Metric::Metric(int value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(std::string value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(double value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(uint32_t value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

boost::variant< int, std::string, double, uint32_t > Metric::getValue() const
{
  return mValue;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
