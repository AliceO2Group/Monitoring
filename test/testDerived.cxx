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

  AliceO2::Monitoring::DerivedMetrics derivedHandler(1000);	
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::AVERAGE);
		
  for (auto const result : results) {
    AliceO2::Monitoring::Metric metric(result.value, name);
    AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
    BOOST_CHECK_EQUAL(derived.getName(), "metricNameAverage");
    BOOST_CHECK_EQUAL(boost::get<double>(derived.getValue()), result.average);
  }
}

BOOST_AUTO_TEST_CASE(derivedRate)
{
  struct RateResults {
    int value;
    int average;
  };
  std::vector<RateResults> results = {{10, 0 }, {20, 100}, {30, 100}, {50, 200}, {60, 100}, {65, 50}, {70, 50}, {80, 100}, {90, 100}, {100, 100}};
  AliceO2::Monitoring::DerivedMetrics derivedHandler(1000);
  std::string name("metricName");
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);

  for (auto const result : results) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      AliceO2::Monitoring::Metric metric(result.value, name);
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricNameRate");
      BOOST_CHECK_CLOSE(boost::get<double>(derived.getValue()), result.average, 1);
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

BOOST_AUTO_TEST_CASE(stringValue)
{

} 

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
