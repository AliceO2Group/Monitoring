#include "Monitoring/DerivedMetrics.h"
#include "../src/Exceptions/MonitoringInternalException.h"
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#define BOOST_TEST_MODULE Test Monitoring DerivedMetrics
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(derivedRateInt)
{
  struct RateResults {
    int value;
    int rate;
  };
  std::vector<RateResults> results = {{10, 0 }, {20, 100}, {30, 100}, {50, 200}, {60, 100}, {65, 50}, {70, 50}, {80, 100}, {90, 100}, {100, 100}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricInt");

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      o2::monitoring::Metric derived = derivedHandler.calculateRate(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricIntRate");
      BOOST_WARN_CLOSE(boost::get<double>(derived.getValue()), result.rate, 1.0);
    } catch(MonitoringInternalException &e) {
      BOOST_TEST(e.what() == std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(derivedRateDouble) {
  struct RateResults {
    double value;
    double rate;
  };
  std::vector<RateResults> results = {{1.2, 0 }, {11.2, 100}, {21.2, 100}, {41.2, 200}, {51.2, 100}, {61, 98}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricDouble");

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      o2::monitoring::Metric derived = derivedHandler.calculateRate(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricDoubleRate");
      BOOST_WARN_CLOSE(boost::get<double>(derived.getValue()), result.rate, 1.0);
    } catch(MonitoringInternalException &e) {
      BOOST_TEST(e.what() == std::string("Not enough values"));
    }
  }
}

BOOST_AUTO_TEST_CASE(derivedRateUint64_t) {
  struct RateResults {
    uint64_t value;
    uint64_t rate;
  };
  std::vector<RateResults> results = {{165535, 0 }, {165545, 100}, {165555, 100}, {165575, 200}, {165585, 100}, {165590, 50}};
  o2::monitoring::DerivedMetrics derivedHandler;
  std::string name("metricUint64_t");
  
  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      o2::monitoring::Metric metric(result.value, name);
      o2::monitoring::Metric derived = derivedHandler.calculateRate(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricUint64_tRate");
      BOOST_WARN_CLOSE(boost::get<double>(derived.getValue()), result.rate, 1.0);
    } catch(MonitoringInternalException &e) {
      BOOST_TEST(e.what() == std::string("Not enough values"));
    }
  }


}

bool exceptionCheck(const MonitoringInternalException &e)
{
  if (e.what() == std::string("Not enough values")) return true;
  if (e.what() == std::string("Division by 0")) return true;
  return false;
}

BOOST_AUTO_TEST_CASE(divisionByZero)
{
  std::string name("test");
  o2::monitoring::DerivedMetrics derivedHandler;
  o2::monitoring::Metric metric(10, name, o2::monitoring::Metric::getCurrentTimestamp());

  derivedHandler.calculateRate(metric);
  BOOST_CHECK_EXCEPTION(derivedHandler.calculateRate(metric), MonitoringInternalException, exceptionCheck);

}

} // namespace Test
} // namespace monitoring
} // namespace o2
