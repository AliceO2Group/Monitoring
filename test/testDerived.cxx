// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "Monitoring/DerivedMetrics.h"
#include "../src/Exceptions/MonitoringException.h"
#include "../src/VariantVisitorAdd.h"
#include "../src/VariantVisitorRate.h"
#include <chrono>
#include <vector>
#include <iostream>

#define BOOST_TEST_MODULE Test Monitoring DerivedMetrics
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2
{
namespace monitoring
{
namespace Test
{

using namespace o2::monitoring;

struct ResultsKv {
  unsigned short int value;
  unsigned short int result;
};

BOOST_AUTO_TEST_CASE(testDerivedRateForInts)
{
  auto timestamp = std::chrono::system_clock::now();
  std::vector<ResultsKv> valResKv = {{10, 0}, {20, 10}, {30, 10}, {50, 20}, {60, 10}, {65, 5}, {70, 5}, {80, 10}, {90, 10}};
  DerivedMetrics derivedHandler;
  std::string name("metricInt");

  for (auto const valRes : valResKv) {
    try {
      Metric metric(name, Metric::DefaultVerbosity, timestamp);
      metric.addValue(valRes.value, "value");
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricInt");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_CHECK_EQUAL(std::get<double>(metric.getValues().back().second), valRes.result);
      timestamp += std::chrono::seconds(1);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(testDerivedRateForIntsAndResetValue)
{
  std::vector<ResultsKv> valResKv = {{10, 0}, {20, 10}, {30, 10}, {50, 20}, {0, 0}, {10, 10}, {20, 10}, {30, 10}, {50, 20}};
  DerivedMetrics derivedHandler;
  std::string name("metricInt");
  auto timestamp = std::chrono::system_clock::now();

  for (auto const valRes : valResKv) {
    try {
      Metric metric(name, Metric::DefaultVerbosity, timestamp);
      metric.addValue(valRes.value, "value");
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricInt");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_CHECK_EQUAL(std::get<double>(metric.getValues().back().second), valRes.result);
      timestamp += std::chrono::seconds(1);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

struct ResultsKvDouble {
  double value;
  double result;
};

BOOST_AUTO_TEST_CASE(testDerivedRateForDoubles)
{
  std::vector<ResultsKvDouble> valResKv = {{1.2, 0}, {11.2, 10}, {21.2, 10}, {41.2, 20}, {51.2, 10}, {61, 9.8}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricDouble");
  auto timestamp = std::chrono::system_clock::now();

  for (auto const valRes : valResKv) {
    try {
      Metric metric(name, Metric::DefaultVerbosity, timestamp);
      metric.addValue(valRes.value, "value");
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricDouble");
      BOOST_CHECK_CLOSE(std::get<double>(metric.getValues().back().second), valRes.result, 0.1);
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      timestamp += std::chrono::seconds(1);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(testDerivedRateForDoublesWhenUsingTags)
{
  std::vector<ResultsKvDouble> valResKv1 = {{1.2, 0}, {11.2, 10}, {21.2, 10}, {41.2, 20}, {51.2, 10}, {61, 9.8}};
  std::vector<ResultsKvDouble> valResKv2 = {{0.5, 0}, {5.5, 5},   {10.5, 5},  {20.5, 10}, {40.5, 20}, {45.5, 5}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricDouble");
  auto timestamp = std::chrono::system_clock::now();

  for (std::size_t i = 0; i < valResKv1.size(); i++) {
    try {
      Metric metric1(name, Metric::DefaultVerbosity, timestamp);
      metric1.addValue(valResKv1[i].value, "value");

      Metric metric2(name, Metric::DefaultVerbosity, timestamp);
      metric2.addValue(valResKv2[i].value, "value");
      metric2.addTag(o2::monitoring::tags::Key::Subsystem, o2::monitoring::tags::Value::Readout);

      derivedHandler.process(metric1, DerivedMetricMode::RATE);
      derivedHandler.process(metric2, DerivedMetricMode::RATE);

      BOOST_CHECK_CLOSE(std::get<double>(metric1.getValues().back().second), valResKv1[i].result, 0.1);
      BOOST_CHECK_CLOSE(std::get<double>(metric2.getValues().back().second), valResKv2[i].result, 0.1);
      timestamp += std::chrono::seconds(1);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

struct ResultsKvUint64_t {
  uint64_t value;
  uint64_t result;
};

BOOST_AUTO_TEST_CASE(testDerivedRateForUint64_t)
{
  std::vector<ResultsKvUint64_t> valResKv = {{165535, 0}, {165545, 10}, {165555, 10}, {165575, 20}, {165585, 10}, {165590, 5}};
  DerivedMetrics derivedHandler;
  std::string name("metricUint64_t");
  auto timestamp = std::chrono::system_clock::now();

  for (auto const valRes : valResKv) {
    try {
      Metric metric(name, Metric::DefaultVerbosity, timestamp);
      metric.addValue(valRes.value, "value");
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricUint64_t");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_CHECK_EQUAL(std::get<double>(metric.getValues().back().second), valRes.result);
      timestamp += std::chrono::seconds(1);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

bool exceptionCheck(const MonitoringException& e)
{
  if (e.what() == std::string("Not enough values"))
    return true;
  if (e.what() == std::string("Division by 0"))
    return true;
  return false;
}

BOOST_AUTO_TEST_CASE(divisionByZero)
{
  std::string name("test");
  o2::monitoring::DerivedMetrics derivedHandler;
  o2::monitoring::Metric metric(10, name);

  derivedHandler.process(metric, DerivedMetricMode::RATE);
  BOOST_CHECK_EXCEPTION(derivedHandler.process(metric, DerivedMetricMode::RATE), MonitoringException, exceptionCheck);
}

BOOST_AUTO_TEST_CASE(derivedIncrementInt)
{
  std::vector<ResultsKv> results = {{10, 10}, {20, 30}, {30, 60}, {50, 110}, {60, 170}, {65, 235}, {70, 305}, {80, 385}, {90, 475}, {100, 575}};
  DerivedMetrics derivedHandler;
  std::string name("metricInt");
  for (auto const result : results) {
    Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_EQUAL(std::get<int>(metric.getValues().back().second), result.result);
    BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_increment");
  }
}

BOOST_AUTO_TEST_CASE(derivedIncrementUint64_t)
{
  std::vector<ResultsKvUint64_t> results = {{5, 5}, {165535, 165540}, {15, 165555}, {50, 165605}, {165590, 331195}};
  DerivedMetrics derivedHandler;
  std::string name("metricUint64_t");
  for (auto const result : results) {
    Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_EQUAL(std::get<uint64_t>(metric.getValues().back().second), result.result);
    BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_increment");
  }
}

BOOST_AUTO_TEST_CASE(derivedIncrementDouble)
{
  std::vector<ResultsKvDouble> results = {{1.2, 1.2}, {11.2, 12.4}, {21.2, 33.6}, {41.2, 74.8}, {51.2, 126}, {61, 187}};
  DerivedMetrics derivedHandler;
  std::string name("metricDouble");
  for (auto const result : results) {
    Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_CLOSE(std::get<double>(metric.getValues().back().second), result.result, 0.01);
    BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_increment");
  }
}

BOOST_AUTO_TEST_CASE(testBoostVisitor)
{
  {
    std::variant<int, std::string, double, uint64_t> a = 10;
    std::variant<int, std::string, double, uint64_t> b = 11;
    auto value = std::visit(VariantVisitorAdd{}, a, b);
  }
  {
    std::variant<int, std::string, double, uint64_t> a = 10;
    std::variant<int, std::string, double, uint64_t> b = 10.10;
    BOOST_CHECK_THROW(std::visit(VariantVisitorAdd{}, a, b), o2::monitoring::MonitoringException);
    BOOST_CHECK_THROW(std::visit(VariantVisitorRate(1000), a, b), o2::monitoring::MonitoringException);
  }
}

} // namespace Test
} // namespace monitoring
} // namespace o2
