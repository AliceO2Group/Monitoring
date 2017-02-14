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

Metric DerivedMetrics::calculateRate(std::string name)
{
  auto search = mCache.find(name);
  int size = search->second.size();
  if (search == mCache.end() || size < 2) throw std::logic_error("Not enough values");
  std::chrono::duration<float> timestampDifference = (search->second.at(size - 1).getTimestamp() 
                                                   - search->second.at(size - 2).getTimestamp());
  

  double last = boost::lexical_cast<double>(search->second.at(size - 1).getValue());
  double beforelast = boost::lexical_cast<double>(search->second.at(size - 2).getValue());
  // disallow dividing by 0
  if (timestampDifference.count() == 0) throw std::logic_error("Division by 0");
  double rate = (last - beforelast ) / timestampDifference.count();
  return Metric{rate, name + "Rate"};
}

Metric DerivedMetrics::calculateAverage(std::string name)
{
  double total = 0;
  for (auto& m : mCache.at(name)) {
    total += boost::lexical_cast<double>(m.getValue());
  }
  double average = total / mCache.at(name).size();
  return Metric{average, name + "Average"};
}

Metric DerivedMetrics::processMetric(Metric& metric)
{
  std::string name = metric.getName();
  if (!isRegistered(name) || metric.getType() == MetricType::STRING) {
    throw std::logic_error("Not able to calculate derived value");
  }
  auto search = mCache.find(name);
  // create vector if this is first metric of this kind
  if (search == mCache.end()) {
    mCache.emplace(std::make_pair(name, std::vector<Metric>()));
  }
  // remove first value if vector too large
  if (mCache.at(name).size() > mMaxVectorSize) {
    mCache.at(name).erase( mCache.at(name).begin() );
  }
  mCache[name].push_back(metric);

  auto derived = mRegistered.find(name);
  if (derived->second == DerivedMetricMode::RATE) {
    return calculateRate(name);
  }
  else if (derived->second == DerivedMetricMode::AVERAGE)  {
    return calculateAverage(name);
  } else {
    throw std::logic_error("Processing mode not supported");
  }
}

} // namespace Monitoring
} // namespace AliceO2
