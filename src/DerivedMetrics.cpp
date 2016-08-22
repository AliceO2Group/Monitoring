#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/InfoLoggerBackend.h"
#include "Monitoring/MonInfoLogger.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

void DerivedMetrics::registerMetric(DerivedMetricMode mode, std::string name) {
  registered.insert(std::pair<std::string, DerivedMetricMode>(name, mode));
  MonInfoLogger::GetInstance() << "Monitoring : Metric " << name << " added to derived metrics" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

bool DerivedMetrics::isRegistered(std::string name) {
  auto search = registered.find(name);
  return (search != registered.end());
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::calculateRate(std::string name, T) {
  auto search = cache.find(name);
  if (search != cache.end()) {
    int size = search->second.size();
    if (size >= 2) {
      std::chrono::duration<float> timestampDifferenct = search->second[size - 1]->getTimestamp() - search->second[size - 2]->getTimestamp();
      T last = boost::get<T>(search->second[size - 1]->getValue());
      T beforelast = boost::get<T>(search->second[size - 2]->getValue());
      // disallow dividing by 0
      if (timestampDifferenct.count() == 0) return nullptr;
      T rate = (last - beforelast ) / timestampDifferenct.count();
      return std::unique_ptr<Metric>(new Metric(rate, name + "Rate", 
                                     cache[name].back()->getEntity(), cache[name].back()->getTimestamp()));
    } 
  }
  return nullptr;
}

template <>
std::unique_ptr<Metric> DerivedMetrics::calculateRate(std::string, std::string) {
  return nullptr;
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::calculateAverage(std::string name, T) {
  T total = 0;
  for (auto& m : cache[name]) {
    total += boost::get<T>(m->getValue());
  }
  T average = (T) total / cache[name].size();
  return std::unique_ptr<Metric>(new Metric(average, name + "Average",
                                 cache[name].back()->getEntity(), cache[name].back()->getTimestamp()));
}

template <>
std::unique_ptr<Metric> DerivedMetrics::calculateAverage(std::string, std::string) {
  return nullptr;
}

template<typename T>
std::unique_ptr<Metric> DerivedMetrics::processMetric(T value, std::string name, std::string entity, 
                                                      std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  auto search = cache.find(name);
  // create vector if this is first metric of this kind
  if (search == cache.end()) {
    cache.insert(std::pair<std::string, std::vector<std::unique_ptr<Metric>>>(name, std::vector<std::unique_ptr<Metric>>()));
  }
  // remove first value if vector too large
  if (cache[name].size() > MAX_VECTOR_SIZE) {
    cache[name].erase( cache[name].begin() );
  }
  cache[name].emplace_back(std::unique_ptr<Metric>(new Metric(value, name, entity, timestamp)));

  auto derived = registered.find(name);
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
template std::unique_ptr<Metric> DerivedMetrics::processMetric(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
template std::unique_ptr<Metric> DerivedMetrics::processMetric(double value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
template std::unique_ptr<Metric> DerivedMetrics::processMetric(std::string value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
template std::unique_ptr<Metric> DerivedMetrics::processMetric(uint32_t value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

