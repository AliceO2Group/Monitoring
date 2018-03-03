///
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "Monitoring/DerivedMetrics.h"
#include "Exceptions/MonitoringInternalException.h"
#include <boost/lexical_cast.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "MonLogger.h"
#include "VariantVisitorRate.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

Metric DerivedMetrics::calculateRate(Metric& metric)
{
  std::string name = metric.getName();
  if (metric.getType() == MetricType::STRING) {
    throw MonitoringInternalException("DerivedMetrics/ProcessMetric", "Not able to process string values");
  }

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
    throw MonitoringInternalException("DerivedMetrics/Calculate rate", "Division by 0");
  }

  auto rate =  boost::apply_visitor(VariantVisitorRate(timestampCount), current, previous);
  return Metric{rate, name + "Rate"};
}

} // namespace monitoring
} // namespace o2
