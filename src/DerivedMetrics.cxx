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
  mRegistered.emplace(std::pair<std::string, DerivedMetricMode>(name, mode));
  MonInfoLogger::Info() << "Monitoring : Metric " << name << " added to derived metrics" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

bool DerivedMetrics::isRegistered(std::string name)
{
  auto search = mRegistered.find(name);
  return (search != mRegistered.end());
}

std::unique_ptr<Metric> DerivedMetrics::calculateRate(std::string name)
{
  /*auto search = mCache.find(name);
  if (search != mCache.end()) {
    int size = search->second.size();
    if (size >= 2) {
      std::chrono::duration<float> timestampDifference = (search->second.at(size - 1)->getTimestamp() 
                                                        - search->second.at(size - 2)->getTimestamp());
      T last = boost::get<T>(search->second.at(size - 1)->getValue());
      T beforelast = boost::get<T>(search->second.at(size - 2)->getValue());
      // disallow dividing by 0
      if (timestampDifference.count() == 0) return nullptr;
      T rate = (last - beforelast ) / timestampDifference.count();
      return std::make_unique<Metric>(rate, name + "Rate", 
                                     mCache.at(name).back()->getEntity(), mCache.at(name).back()->getTimestamp());
    } 
  }*/
  return nullptr;
}

std::unique_ptr<Metric> DerivedMetrics::calculateAverage(std::string name)
{
  /*T total = 0;
  for (auto& m : mCache.at(name)) {
    total += boost::get<T>(m->getValue());
  }
  T average = (T) total / mCache.at(name).size();
  return std::make_unique<Metric>(average, name + "Average",
                                 mCache.at(name).back()->getEntity(), mCache.at(name).back()->getTimestamp());*/
  return nullptr;
}

std::unique_ptr<Metric> DerivedMetrics::processMetric(Metric& metric)
{
   std::string name = metric.getName();
   auto search = mCache.find(name);
  // create vector if this is first metric of this kind
  if (search == mCache.end()) {
    mCache.emplace(std::make_pair(name, std::vector<std::unique_ptr<Metric>>()));
  }
  // remove first value if vector too large
  if (mCache.at(name).size() > mMaxVectorSize) {
    mCache.at(name).erase( mCache.at(name).begin() );
  }
  mCache[name].push_back(std::make_unique<Metric>(metric));

  auto derived = mRegistered.find(name);
  if (derived->second == DerivedMetricMode::RATE) {
    return calculateRate(name);
  }
  else if (derived->second == DerivedMetricMode::AVERAGE)  {
    return calculateAverage(name);
  } else {
    MonInfoLogger::Warning() << "Monitoring : Processing mode incorrect for metric " << name << AliceO2::InfoLogger::InfoLogger::endm;
    return nullptr;
  }
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

