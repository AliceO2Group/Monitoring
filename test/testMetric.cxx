#include "../include/Monitoring/Metric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE Test Monitoring Metric
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  int value = 10;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name);

  BOOST_CHECK_EQUAL(metricInstance.getName(), "metric name");
}


BOOST_AUTO_TEST_CASE(retrieveInt) {
  int value = 10;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<int>(metricInstance.getValue()), 10);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 0);
}

BOOST_AUTO_TEST_CASE(retrieveDouble) {
  double value = 1.11;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<double>(metricInstance.getValue()), 1.11);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 2);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
  std::string value = "testString";
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<std::string>(metricInstance.getValue()), "testString");
  BOOST_CHECK_EQUAL(metricInstance.getType(), 1);
}

BOOST_AUTO_TEST_CASE(retrieveUnsignedLongLong)
{
  uint64_t value = 10000000000000LL;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<uint64_t>(metricInstance.getValue()), 10000000000000LL);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 3);
}

bool is_critical(const boost::bad_get&) { return true; }

BOOST_AUTO_TEST_CASE(retrieveWrongType) {
  double value = 1.11;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );
  BOOST_CHECK_EXCEPTION(boost::get<std::string>(metricInstance.getValue()), boost::bad_get, is_critical);
}

BOOST_AUTO_TEST_CASE(tags) {
  o2::monitoring::Metric metric = o2::monitoring::Metric{10, "myMetric"}.addTags({o2::monitoring::tags::Detector::TPC, o2::monitoring::tags::Detector::TRD});
  BOOST_CHECK_EQUAL("detector=TPC,detector=TRD", metric.getTags());
}

BOOST_AUTO_TEST_CASE(metricCopy) {
  o2::monitoring::Metric metric = o2::monitoring::Metric{10, "myMetric"}.addTags({o2::monitoring::tags::Detector::TPC, o2::monitoring::tags::Detector::TRD});
  o2::monitoring::Metric assigned{1, "assingedMetric"};
  auto copied = metric;
  assigned = metric;
  BOOST_CHECK_EQUAL(boost::get<int>(copied.getValue()), 10);
  BOOST_CHECK_EQUAL(copied.getName(), "myMetric");

  BOOST_CHECK_EQUAL(boost::get<int>(assigned.getValue()), 10);
  BOOST_CHECK_EQUAL(assigned.getName(), "myMetric");

  BOOST_CHECK_EQUAL("detector=TPC,detector=TRD", copied.getTags());
}

} // namespace Test
} // namespace monitoring
} // namespace o2
