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
#include <thread>
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

BOOST_AUTO_TEST_CASE(derivedRateInt)
{
  struct RateResults {
    int value;
    int rate;
  };
  std::vector<RateResults> results = {{10, 0}, {20, 100}, {30, 100}, {50, 200}, {60, 100}, {65, 50}, {70, 50}, {80, 100}, {90, 100}, {100, 100}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricInt");

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricInt");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_WARN_CLOSE(std::get<double>(metric.getValues().back().second), result.rate, 1.0);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(derivedRateInt_newRun)
{
  struct RateResults {
    int value;
    int rate;
  };
  std::vector<RateResults> results = {{10, 0}, {20, 100}, {30, 100}, {50, 200}, {0, 0}, {10, 100}, {20, 100}, {30, 100}, {50, 200}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricInt");

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricInt");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_WARN_CLOSE(std::get<double>(metric.getValues().back().second), result.rate, 1.0);
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(derivedRateDouble)
{
  struct RateResults {
    double value;
    double rate;
  };
  std::vector<RateResults> results = {{1.2, 0}, {11.2, 100}, {21.2, 100}, {41.2, 200}, {51.2, 100}, {61, 98}};
  std::vector<RateResults> resultsTagged = {{0.5, 0}, {5.5, 50}, {10.5, 50}, {20.5, 100}, {40.5, 200}, {45.5, 50}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricDouble");
  for (std::size_t i = 0; i < results.size(); i++) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(results[i].value, name);
      o2::monitoring::Metric metricTagged = Metric{resultsTagged[i].value, name}.addTag(o2::monitoring::tags::Key::Subsystem, o2::monitoring::tags::Value::Readout);
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      derivedHandler.process(metricTagged, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricDouble");
      BOOST_WARN_CLOSE(std::get<double>(metric.getValues().back().second), results[i].rate, 1.0);
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_WARN_CLOSE(std::get<double>(metricTagged.getValues().back().second), resultsTagged[i].rate, 1.0);
      BOOST_CHECK_EQUAL(metricTagged.getValues().back().first, "value_rate");
    } catch (MonitoringException& e) {
      BOOST_CHECK_EQUAL(e.what(), std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(derivedRateUint64_t)
{
  struct RateResults {
    uint64_t value;
    uint64_t rate;
  };
  std::vector<RateResults> results = {{165535, 0}, {165545, 100}, {165555, 100}, {165575, 200}, {165585, 100}, {165590, 50}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricUint64_t");

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      derivedHandler.process(metric, DerivedMetricMode::RATE);
      BOOST_CHECK_EQUAL(metric.getName(), "metricUint64_t");
      BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_rate");
      BOOST_WARN_CLOSE(std::get<double>(metric.getValues().back().second), result.rate, 1.0);
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
  struct IncrementResults {
    int value;
    int increment;
  };
  std::vector<IncrementResults> results = {{10, 10}, {20, 30}, {30, 60}, {50, 110}, {60, 170}, {65, 235}, {70, 305}, {80, 385}, {90, 475}, {100, 575}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricInt");
  for (auto const result : results) {
    o2::monitoring::Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_EQUAL(std::get<int>(metric.getValues().back().second), result.increment);
    BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_increment");
  }
}

BOOST_AUTO_TEST_CASE(derivedIncrementUint64_t)
{
  struct IncrementResults {
    uint64_t value;
    uint64_t rate;
  };
  std::vector<IncrementResults> results = {{5, 5}, {165535, 165540}, {15, 165555}, {50, 165605}, {165590, 331195}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricUint64_t");
  for (auto const result : results) {
    o2::monitoring::Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_EQUAL(std::get<uint64_t>(metric.getValues().back().second), result.rate);
    BOOST_CHECK_EQUAL(metric.getValues().back().first, "value_increment");
  }
}

BOOST_AUTO_TEST_CASE(derivedIncrementDouble)
{
  struct IncrementResults {
    double value;
    double rate;
  };
  std::vector<IncrementResults> results = {{1.2, 1.2}, {11.2, 12.4}, {21.2, 33.6}, {41.2, 74.8}, {51.2, 126}, {61, 187}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricDouble");
  for (auto const result : results) {
    o2::monitoring::Metric metric(result.value, name);
    derivedHandler.process(metric, DerivedMetricMode::INCREMENT);
    BOOST_CHECK_CLOSE(std::get<double>(metric.getValues().back().second), result.rate, 0.01);
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
