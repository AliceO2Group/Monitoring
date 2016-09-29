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
  std::string entity("metricEntity");

  AliceO2::Monitoring::Core::DerivedMetrics derivedHandler(1000);	
  derivedHandler.registerMetric(name, AliceO2::Monitoring::Core::DerivedMetricMode::AVERAGE);
		
  for (auto const result : results) {
    std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
    std::unique_ptr<AliceO2::Monitoring::Core::Metric> derived = derivedHandler.processMetric(result.value, name, entity, timestamp);
    if (derived == nullptr) continue;
    BOOST_CHECK_EQUAL(derived->getName(), "metricNameAverage");
    BOOST_CHECK_EQUAL(derived->getEntity(), "metricEntity");
    BOOST_CHECK_EQUAL(boost::get<int>(derived->getValue()), result.average);
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

  AliceO2::Monitoring::Core::DerivedMetrics derivedHandler(1000);
  derivedHandler.registerMetric(name, AliceO2::Monitoring::Core::DerivedMetricMode::RATE);

  for (auto const result : results) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
    std::unique_ptr<AliceO2::Monitoring::Core::Metric> derived = derivedHandler.processMetric(result.value, name, entity, timestamp);
    if (derived == nullptr) continue;
    BOOST_CHECK_EQUAL(derived->getName(), "metricNameRate");
    BOOST_CHECK_EQUAL(derived->getEntity(), "metricEntity");
    BOOST_CHECK((boost::get<int>(derived->getValue()) == result.average) || (boost::get<int>(derived->getValue()) == result.average - 1));
  }
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
