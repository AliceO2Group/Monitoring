///
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "Monitoring/DerivedMetrics.h"
#include "Exceptions/MonitoringException.h"
#include <boost/lexical_cast.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "VariantVisitorRate.h"
#include "VariantVisitorAdd.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

Metric DerivedMetrics::process(Metric& metric, DerivedMetricMode mode) {
  const std::map<DerivedMetricMode, std::function<Metric(Metric&)>> map = {
    {
      DerivedMetricMode::INCREMENT, [this](Metric& metric) {
        auto tags = metric.getTags();
        std::string key = metric.getName() + std::string(tags.begin(), tags.end());
        auto search = mStorage.find(key);
        if (search != mStorage.end()) {
          auto currentValue = metric.getValue();
          auto storedValue = search->second.getValue();
          auto value = boost::apply_visitor(VariantVisitorAdd(), currentValue, storedValue);
          mStorage.erase(search);
          Metric result = Metric{value, metric.getName() + "Increment", metric.getVerbosity()}.addTags(std::move(tags));
          mStorage.insert(std::make_pair(key, result));
          return result;
        }
        mStorage.insert(std::make_pair(key, metric));
        return metric;
      }
    }, {
      DerivedMetricMode::RATE, [this](Metric& metric) {
        // disallow string
        auto tags = metric.getTags();
        std::string key = metric.getName() + std::string(tags.begin(), tags.end());
        if (metric.getType() == MetricType::STRING) {
          throw MonitoringException("DerivedMetrics", "Not able to process string values");
        }

        // search for previous value
        auto search = mStorage.find(key);
        if (search == mStorage.end()) {
        mStorage.insert(std::make_pair(key, metric));
          return Metric{(double) 0.0, metric.getName() + "Rate", metric.getVerbosity()}.addTags(std::move(tags));
        }

        auto timestampDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
          metric.getTimestamp()
          - search->second.getTimestamp()
        );
        int timestampCount = timestampDifference.count();
        // disallow dividing by 0
        if (timestampCount == 0) {
          throw MonitoringException("DerivedMetrics", "Division by 0");
        }

        auto current = metric.getValue();
        auto previous = search->second.getValue();
        auto rate =  boost::apply_visitor(VariantVisitorRate(timestampCount), current, previous);

        // swap metrics
        mStorage.erase(key);
        mStorage.insert(std::make_pair(key, metric));
        return Metric{rate, metric.getName() + "Rate", metric.getVerbosity()}.addTags(std::move(tags));
      }
    }
  };
  auto iterator = map.find(mode);
  if (iterator == map.end()) {
    throw MonitoringException("DerivedMetrics", "Unknown mode");
  }
  return iterator->second(metric);
}

} // namespace monitoring
} // namespace o2
