#include "../include/Monitoring/Metric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE Test Monitoring Metric
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  int value = 10;
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name);

  BOOST_CHECK_EQUAL(metricInstance.getName(), "metric name");
}


BOOST_AUTO_TEST_CASE(retrieveInt) {
  int value = 10;
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<int>(metricInstance.getValue()), 10);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 0);
}

BOOST_AUTO_TEST_CASE(retrieveDouble) {
  double value = 1.11;
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<double>(metricInstance.getValue()), 1.11);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 2);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
  std::string value = "testString";
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<std::string>(metricInstance.getValue()), "testString");
  BOOST_CHECK_EQUAL(metricInstance.getType(), 1);
}

BOOST_AUTO_TEST_CASE(retrieveUnsignedLongLong)
{
  uint64_t value = 10000000000000LL;
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<uint64_t>(metricInstance.getValue()), 10000000000000LL);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 3);
}

bool is_critical(const boost::bad_get&) { return true; }

BOOST_AUTO_TEST_CASE(retrieveWrongType) {
  double value = 1.11;
  std::string name("metric name");
  AliceO2::Monitoring::Metric metricInstance(value,  name );
  BOOST_CHECK_EXCEPTION(boost::get<std::string>(metricInstance.getValue()), boost::bad_get, is_critical);
}

BOOST_AUTO_TEST_CASE(tags) {
  AliceO2::Monitoring::Metric metric = AliceO2::Monitoring::Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}});
  std::vector<Tag> tags = metric.getTags();
  for (auto const& tag: tags) {
    BOOST_TEST(tag.name.find("tag") != std::string::npos);
    BOOST_TEST(tag.value.find("value") != std::string::npos);
  }
}



} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
