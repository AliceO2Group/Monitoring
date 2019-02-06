#include "../include/Monitoring/Metric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE Test Monitoring Metric
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

using namespace o2::monitoring;

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  int value = 10;
  Metric metricInstance(value,  "metric name");

  BOOST_CHECK_EQUAL(metricInstance.getName(), "metric name");
}

BOOST_AUTO_TEST_CASE(constCharName)
{
  const char* name = "a name";
  Metric metricInstance(10,  name);
  BOOST_CHECK_EQUAL(metricInstance.getName(), "a name");
}

BOOST_AUTO_TEST_CASE(retrieveInt) {
  int value = 10;
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<int>(metricInstance.getValue()), 10);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 0);
}

BOOST_AUTO_TEST_CASE(retrieveDouble) {
  double value = 1.11;
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<double>(metricInstance.getValue()), 1.11);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 2);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
  std::string value = "testString";
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<std::string>(metricInstance.getValue()), "testString");
  BOOST_CHECK_EQUAL(metricInstance.getType(), 1);
}

BOOST_AUTO_TEST_CASE(retrieveUnsignedLongLong)
{
  uint64_t value = 10000000000000LL;
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(boost::get<uint64_t>(metricInstance.getValue()), 10000000000000LL);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 3);
}

bool is_critical(const boost::bad_get&) { return true; }

BOOST_AUTO_TEST_CASE(retrieveWrongType) {
  double value = 1.11;
  std::string name("metric name");
  Metric metricInstance(value,  name );
  BOOST_CHECK_EXCEPTION(boost::get<std::string>(metricInstance.getValue()), boost::bad_get, is_critical);
}

BOOST_AUTO_TEST_CASE(tags) {
  Metric metric = Metric{10, "myMetric"}.addTag(o2::monitoring::tags::Key::Detector, o2::monitoring::tags::Value::TRD);
  auto tags = metric.getTags();
  int sum = 0;
  for (auto const& tag: tags) {
    sum += tag.second;
  }
  BOOST_CHECK_EQUAL(sum, 14);
}

BOOST_AUTO_TEST_CASE(customCopyConstructor) {
  Metric metric = Metric{10, "myMetric"}.addTag(o2::monitoring::tags::Key::Detector, 123).addTag(o2::monitoring::tags::Key::Detector, o2::monitoring::tags::Value::TRD);
  Metric assigned{1, "assingedMetric"};
  auto copied = metric;
  assigned = metric;
  BOOST_CHECK_EQUAL(boost::get<int>(copied.getValue()), 10);
  BOOST_CHECK_EQUAL(copied.getName(), "myMetric");

  BOOST_CHECK_EQUAL(boost::get<int>(assigned.getValue()), 10);
  BOOST_CHECK_EQUAL(assigned.getName(), "myMetric");

  auto tags = copied.getTags();
  int sum = 0;
  for (auto const& tag: tags) {
    sum += tag.second;
  }
  BOOST_CHECK_EQUAL(sum, -109);
}

BOOST_AUTO_TEST_CASE(verbosity) {
  Metric{10, "myMetric", Verbosity::Prod};
}

} // namespace Test
} // namespace monitoring
} // namespace o2
