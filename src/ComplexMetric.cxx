///
/// \file ComplexMetric.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ComplexMetric.h"

#include <iostream>
#include <chrono>
#include <memory>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

ComplexMetric::ComplexMetric(int value, const std::string& name) :
  Metric(value, name)
{}

ComplexMetric::ComplexMetric(std::string value, const std::string& name) :
  Metric(value, name)
{}

ComplexMetric::ComplexMetric(double value, const std::string& name) :
  Metric(value, name)
{}

ComplexMetric::ComplexMetric(uint64_t value, const std::string& name) :
  Metric(value, name)
{}

ComplexMetric::ComplexMetric(boost::variant< int, std::string, double, uint64_t > value, const std::string& name) :
  Metric(value, name)
{}

void ComplexMetric::resetTimestamp()
{
   mTimestamp = Metric::getCurrentTimestamp();
}

ComplexMetric& ComplexMetric::operator=(const boost::variant< int, std::string, double, uint64_t >& value) {
  mValue = value;
  return *this;
}

} // namespace monitoring
} // namespace o2
