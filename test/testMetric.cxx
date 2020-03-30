#include "../include/Monitoring/Metric.h"
#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE Test Monitoring Metric
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2
{
namespace monitoring
{
namespace Test
{

using namespace o2::monitoring;

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  int value = 10;
  Metric metricInstance(value, "metric name");
  BOOST_CHECK_EQUAL(metricInstance.getName(), "metric name");
}

BOOST_AUTO_TEST_CASE(constCharName)
{
  const char* name = "a name";
  Metric metricInstance(10, name);
  BOOST_CHECK_EQUAL(metricInstance.getName(), "a name");
  BOOST_CHECK_EQUAL(metricInstance.getFirstValue().first, "value");
  Metric metricInstance2("metric name");
  metricInstance2.addValue(10, name);
  BOOST_CHECK_EQUAL(metricInstance2.getName(), "metric name");
  BOOST_CHECK_EQUAL(metricInstance2.getFirstValue().first, "a name");
}

BOOST_AUTO_TEST_CASE(retrieveInt)
{
  int value = 10;
  std::string name("metric name");
  Metric metricInstance(value, name);

  BOOST_CHECK_EQUAL(std::get<int>(metricInstance.getFirstValue().second), 10);
  BOOST_CHECK_EQUAL(metricInstance.getFirstValue().first, "value");
  BOOST_CHECK(std::holds_alternative<int>(metricInstance.getFirstValue().second));
}

BOOST_AUTO_TEST_CASE(retrieveDouble)
{
  double value = 1.11;
  double value2 = 2.22;
  std::string name("metric name");
  Metric metricInstance(value, name);
  BOOST_CHECK_EQUAL(std::get<double>(metricInstance.getFirstValue().second), 1.11);
  BOOST_CHECK_EQUAL(metricInstance.getFirstValue().first, "value");
  BOOST_CHECK(std::holds_alternative<double>(metricInstance.getFirstValue().second));
  metricInstance.addValue(value2, "double");
  BOOST_CHECK_EQUAL(metricInstance.getValues().back().first, "double");
  BOOST_CHECK(std::holds_alternative<double>(metricInstance.getValues().back().second));
  BOOST_CHECK_EQUAL(std::get<double>(metricInstance.getValues().back().second), 2.22);
}

BOOST_AUTO_TEST_CASE(retrieveString)
{
  std::string value = "testString";
  std::string value2 = "testString2";
  std::string name("metric name");
  Metric metricInstance(value, name);

  BOOST_CHECK_EQUAL(std::get<std::string>(metricInstance.getFirstValue().second), "testString");
  BOOST_CHECK_EQUAL(metricInstance.getFirstValue().first, "value");
  BOOST_CHECK(std::holds_alternative<std::string>(metricInstance.getFirstValue().second));
  metricInstance.addValue(value2, "string");
  BOOST_CHECK_EQUAL(metricInstance.getValues().back().first, "string");
  BOOST_CHECK(std::holds_alternative<std::string>(metricInstance.getValues().back().second));
  BOOST_CHECK_EQUAL(std::get<std::string>(metricInstance.getValues().back().second), "testString2");
}

BOOST_AUTO_TEST_CASE(retrieveUnsignedLongLong)
{
  uint64_t value = 10000000000000LL;
  uint64_t value2 = 10000000000001LL;
  std::string name("metric name");
  Metric metricInstance(value, name);
  BOOST_CHECK_EQUAL(std::get<uint64_t>(metricInstance.getFirstValue().second), 10000000000000LL);
  BOOST_CHECK_EQUAL(metricInstance.getFirstValue().first, "value");
  BOOST_CHECK(std::holds_alternative<uint64_t>(metricInstance.getFirstValue().second));
  metricInstance.addValue(value2, "uint64_t");
  BOOST_CHECK_EQUAL(metricInstance.getValues().back().first, "uint64_t");
  BOOST_CHECK(std::holds_alternative<uint64_t>(metricInstance.getValues().back().second));
  BOOST_CHECK_EQUAL(std::get<uint64_t>(metricInstance.getValues().back().second), 10000000000001LL);
}

bool is_critical(const std::bad_variant_access&) { return true; }

BOOST_AUTO_TEST_CASE(retrieveWrongType)
{
  double value = 1.11;
  std::string name("metric name");
  o2::monitoring::Metric metricInstance(value, name);
  BOOST_CHECK_EXCEPTION(std::get<std::string>(metricInstance.getFirstValue().second), std::bad_variant_access, is_critical);
}

BOOST_AUTO_TEST_CASE(tags)
{
  Metric metric = Metric{10, "myMetric"}.addTag(o2::monitoring::tags::Key::Detector, o2::monitoring::tags::Value::TRD).addTag(o2::monitoring::tags::Key::Unit, o2::monitoring::tags::Value::Milliseconds);
  auto tags = metric.getTags();
  int sum = 0;
  for (auto const& tag : tags) {
    sum += tag.second;
  }
  BOOST_CHECK_EQUAL(sum, 45);
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

BOOST_AUTO_TEST_CASE(verbosity)
{
  Metric metric = Metric{10, "myMetric", Verbosity::Prod};
  BOOST_CHECK_EQUAL(static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(metric.getVerbosity()),
                    static_cast<std::underlying_type<o2::monitoring::tags::Value>::type>(Verbosity::Prod));
}

} // namespace Test
} // namespace monitoring
} // namespace o2
