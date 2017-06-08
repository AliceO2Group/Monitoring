#include "../include/Monitoring/DerivedMetrics.h"
#include "../src/Exceptions/MonitoringInternalException.h"
#include <chrono>
#include <thread>
#include <vector>

#define BOOST_TEST_MODULE testDerived
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(derivedAverage)
{
  struct AverageResults {
    int value;
    int average;
  };
  std::vector<AverageResults> results = {{10, 10}, {20, 15}, {30, 20}, {40, 25}, {50, 30}, {60, 35}, {70, 40}, {80, 45}, {90, 50}, {100, 55}};
  std::string name("metricName");

  AliceO2::Monitoring::DerivedMetrics derivedHandler(100);	
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::AVERAGE);
		
  for (auto const result : results) {
    AliceO2::Monitoring::Metric metric(result.value, name);
    AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
    BOOST_CHECK_EQUAL(derived.getName(), "metricNameAverage");
    BOOST_CHECK_EQUAL(boost::get<double>(derived.getValue()), result.average);
  }
}

BOOST_AUTO_TEST_CASE(derivedRateInt)
{
  struct RateResults {
    int value;
    int rate;
  };
  std::vector<RateResults> results = {{10, 0 }, {20, 100}, {30, 100}, {50, 200}, {60, 100}, {65, 50}, {70, 50}, {80, 100}, {90, 100}, {100, 100}};
  AliceO2::Monitoring::DerivedMetrics derivedHandler(100);
  std::string name("metricInt");
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      AliceO2::Monitoring::Metric metric(result.value, name);
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricIntRate");
      BOOST_CHECK_EQUAL(boost::get<double>(derived.getValue()), result.rate);
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
  AliceO2::Monitoring::DerivedMetrics derivedHandler(100);
  std::string name("metricDouble");
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      AliceO2::Monitoring::Metric metric(result.value, name);
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricDoubleRate");
      BOOST_CHECK_CLOSE(boost::get<double>(derived.getValue()), result.rate, 0.1);
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
  AliceO2::Monitoring::DerivedMetrics derivedHandler(100);
  std::string name("metricUint64_t");
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);
  
  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      AliceO2::Monitoring::Metric metric(result.value, name);
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricUint64_tRate");
      BOOST_CHECK_EQUAL(boost::get<double>(derived.getValue()), result.rate);
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
  AliceO2::Monitoring::DerivedMetrics derivedHandler(1000);
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);

  AliceO2::Monitoring::Metric metric(10, name, AliceO2::Monitoring::Metric::getCurrentTimestamp());

  BOOST_CHECK_EXCEPTION(AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric), MonitoringInternalException, exceptionCheck);
  BOOST_CHECK_EXCEPTION(AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric), MonitoringInternalException, exceptionCheck);

}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
