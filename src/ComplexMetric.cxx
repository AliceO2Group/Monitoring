// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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
