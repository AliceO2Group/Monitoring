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

  BOOST_CHECK_EQUAL(std::get<int>(metricInstance.getValue()), 10);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 0);
}

BOOST_AUTO_TEST_CASE(retrieveDouble) {
  double value = 1.11;
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(std::get<double>(metricInstance.getValue()), 1.11);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 2);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
  std::string value = "testString";
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(std::get<std::string>(metricInstance.getValue()), "testString");
  BOOST_CHECK_EQUAL(metricInstance.getType(), 1);
}

BOOST_AUTO_TEST_CASE(retrieveUnsignedLongLong)
{
  uint64_t value = 10000000000000LL;
  std::string name("metric name");
  Metric metricInstance(value,  name );

  BOOST_CHECK_EQUAL(std::get<uint64_t>(metricInstance.getValue()), 10000000000000LL);
  BOOST_CHECK_EQUAL(metricInstance.getType(), 3);
}

bool is_critical(const std::bad_variant_access&) { return true; }

BOOST_AUTO_TEST_CASE(retrieveWrongType) {
  double value = 1.11;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value,  name );
  BOOST_CHECK_EXCEPTION(std::get<std::string>(metricInstance.getValue()), std::bad_variant_access, is_critical);
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

BOOST_AUTO_TEST_CASE(regexVerbosityPolicy)
{
  Metric::setVerbosityPolicy(Verbosity::Prod, std::regex("myMetric", std::regex::optimize));
  Metric metric = Metric{10, "myMetric"};
  Metric metric2 = Metric{10, "myValue"};
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(metric.getVerbosity()),
                    static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(Verbosity::Prod));
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(metric2.getVerbosity()),
                    static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(Verbosity::Info));
}

BOOST_AUTO_TEST_CASE(verbosity) {
  Metric metric = Metric{10, "myMetric", Verbosity::Prod};
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(metric.getVerbosity()),
                    static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(Verbosity::Prod));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
