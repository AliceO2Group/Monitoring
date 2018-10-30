///
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "Monitoring/DerivedMetrics.h"
#include "Exceptions/MonitoringException.h"
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "VariantVisitorAdd.h"
#include "VariantVisitorRate.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

Metric DerivedMetrics::rate(Metric& metric)
{
  // disallow string
  std::string name = metric.getName();
  if (std::holds_alternative<std::string>(metric.getValue())) {
    throw MonitoringException("DerivedMetrics/ProcessMetric", "Not able to process string values");
  }

  // search for previous value
  auto search = mStorage.find(name);
  if (search == mStorage.end()) {
    mStorage.insert(std::make_pair(name, metric));
    return Metric{(double) 0.0, name + "Rate"};
  }

  auto timestampDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
    metric.getTimestamp()
    - search->second.getTimestamp()
  );
  int timestampCount = timestampDifference.count();
  // disallow dividing by 0
  if (timestampCount == 0) {
    throw MonitoringException("DerivedMetrics/Calculate rate", "Division by 0");
  }

  auto current = metric.getValue();
  auto previous = search->second.getValue();
  auto rate = std::visit(VariantVisitorRate(timestampCount), current, previous);

  // swap metrics
  mStorage.erase(name);
  mStorage.insert(std::make_pair(name, metric));
  return Metric{rate, name + "Rate"};
}

Metric DerivedMetrics::increment(Metric& metric) {
  std::string name = metric.getName();
  auto search = mStorage.find(name);
  if (search != mStorage.end()) {
    auto current = metric.getValue();
    auto previous = search->second.getValue();
    auto value = std::visit(VariantVisitorAdd{}, current, previous);

    mStorage.erase(search);
    Metric result = Metric{value, name};
    mStorage.insert(std::make_pair(name, result));
    return result;
  }
  mStorage.insert(std::make_pair(name, metric));
  return metric;
}

} // namespace monitoring
} // namespace o2
