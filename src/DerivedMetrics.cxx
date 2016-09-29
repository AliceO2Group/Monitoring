///
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/DerivedMetrics.h"

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "InfoLoggerBackend.h"
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core 
{

DerivedMetrics::DerivedMetrics(const unsigned int cacheSize) : mMaxVectorSize(cacheSize) 
{
}

void DerivedMetrics::registerMetric(std::string name, DerivedMetricMode mode)
{
  mRegistered.insert(std::pair<std::string, DerivedMetricMode>(name, mode));
  MonInfoLogger::GetInstance() << "Monitoring : Metric " << name << " added to derived metrics" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

bool DerivedMetrics::isRegistered(std::string name)
{
  auto search = mRegistered.find(name);
  return (search != mRegistered.end());
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::calculateRate(std::string name, T)
{
  auto search = mCache.find(name);
  if (search != mCache.end()) {
    int size = search->second.size();
    if (size >= 2) {
      std::chrono::duration<float> timestampDifference = (search->second[size - 1]->getTimestamp() 
                                                        - search->second[size - 2]->getTimestamp());
      T last = boost::get<T>(search->second[size - 1]->getValue());
      T beforelast = boost::get<T>(search->second[size - 2]->getValue());
      // disallow dividing by 0
      if (timestampDifference.count() == 0) return nullptr;
      T rate = (last - beforelast ) / timestampDifference.count();
      return std::unique_ptr<Metric>(new Metric(rate, name + "Rate", 
                                     mCache[name].back()->getEntity(), mCache[name].back()->getTimestamp()));
    } 
  }
  return nullptr;
}

template <>
std::unique_ptr<Metric> DerivedMetrics::calculateRate(std::string, std::string)
{
  return nullptr;
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::calculateAverage(std::string name, T)
{
  T total = 0;
  for (auto& m : mCache[name]) {
    total += boost::get<T>(m->getValue());
  }
  T average = (T) total / mCache[name].size();
  return std::unique_ptr<Metric>(new Metric(average, name + "Average",
                                 mCache[name].back()->getEntity(), mCache[name].back()->getTimestamp()));
}

template <>
std::unique_ptr<Metric> DerivedMetrics::calculateAverage(std::string, std::string)
{
  return nullptr;
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::processMetric(T value, std::string name, std::string entity, 
                                                      std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  auto search = mCache.find(name);
  // create vector if this is first metric of this kind
  if (search == mCache.end()) {
    mCache.emplace(std::make_pair(name, std::vector<std::unique_ptr<Metric>>()));
  }
  // remove first value if vector too large
  if (mCache[name].size() > mMaxVectorSize) {
    mCache[name].erase( mCache[name].begin() );
  }
  mCache[name].emplace_back(std::unique_ptr<Metric>(new Metric(value, name, entity, timestamp)));

  auto derived = mRegistered.find(name);
  if (derived->second == DerivedMetricMode::RATE) {
    return calculateRate(name, value);
  }
  else if (derived->second == DerivedMetricMode::AVERAGE)  {
        return calculateAverage(name, value);
  } else {
    MonInfoLogger::GetInstance() << "Monitoring : Processing mode incorrect for metric " << name << AliceO2::InfoLogger::InfoLogger::endm;
    return nullptr;
  }
}

template std::unique_ptr<Metric> DerivedMetrics::processMetric(int value, std::string name, std::string entity, 
  std::chrono::time_point<std::chrono::system_clock> timestamp);

template std::unique_ptr<Metric> DerivedMetrics::processMetric(double value, std::string name, std::string entity,
  std::chrono::time_point<std::chrono::system_clock> timestamp);

template std::unique_ptr<Metric> DerivedMetrics::processMetric(std::string value, std::string name, std::string entity,
  std::chrono::time_point<std::chrono::system_clock> timestamp);

template std::unique_ptr<Metric> DerivedMetrics::processMetric(uint32_t value, std::string name, std::string entity,
  std::chrono::time_point<std::chrono::system_clock> timestamp);
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

