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

std::string Metric::getEntity()
{
  return mEntity;
}

std::chrono::time_point<std::chrono::system_clock> Metric::getTimestamp()
{
  return mTimestamp;
}

int Metric::getType()
{
  return mValue.which();
}

std::string Metric::getName()
{
  return mName;
}

Metric::Metric(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(std::string value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(double value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

Metric::Metric(uint32_t value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
  mValue(value), mName(name), mEntity(entity), mTimestamp(timestamp)
{}

boost::variant< int, std::string, double, uint32_t > Metric::getValue()
{
  return mValue;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
