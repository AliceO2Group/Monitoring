#include "../include/Monitoring/DerivedMetrics.h"
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
    try {
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricNameAverage");
      BOOST_CHECK_EQUAL(boost::get<double>(derived.getValue()), result.average);
    } catch (std::logic_error& e) {
       std::cout << e.what() << std::endl;
    }   
  }
}


BOOST_AUTO_TEST_CASE(derivedRate)
{
  struct RateResults {
    int value;
    int average;
  };
  std::vector<RateResults> results = {{10, 0 }, {20, 50}, {30, 50}, {50, 100}, {60, 50}, {65, 25}, {70, 25}, {80, 50}, {90, 50}, {100, 50}};
  std::string name("metricName");
  std::string entity("metricEntity");

  AliceO2::Monitoring::DerivedMetrics derivedHandler(1000);
  derivedHandler.registerMetric(name, AliceO2::Monitoring::DerivedMetricMode::RATE);

  for (auto const result : results) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    AliceO2::Monitoring::Metric metric(result.value, name);
    try {
      AliceO2::Monitoring::Metric derived = derivedHandler.processMetric(metric);
      BOOST_CHECK_EQUAL(derived.getName(), "metricNameRate");
      BOOST_CHECK((boost::get<double>(derived.getValue()) > result.average - 1) || (boost::get<double>(derived.getValue()) < result.average + 1));
    } catch (std::logic_error& e) {
       std::cout << e.what() << std::endl;
    }
  }
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
