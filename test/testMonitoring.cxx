#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE Test Monitoring Colletor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "../include/Monitoring/MonitoringFactory.h"

namespace o2 {
namespace monitoring {
namespace Test {

using Monitoring = o2::monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(createMonitoring)
{
  auto monitoring = Monitoring::Get("stdout://");

  int intMetric = 10; 
  std::string stringMetric("monitoringString");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

  monitoring->addGlobalTag(tags::Key::Name, "sampleDevice");

  monitoring->send({intMetric, "myCrazyMetricI"});
  monitoring->send({stringMetric, "myCrazyMetricS"});
  monitoring->send({doubleMetric, "myCrazyMetricD"});
}

BOOST_AUTO_TEST_CASE(buffering)
{
  auto monitoring = Monitoring::Get("stdout://,flume://localhost:1234");
  monitoring->enableBuffering(10);
  for (int i = 0; i < 25; i++) {
    monitoring->send({10, "myMetricInt"});
  }
  monitoring->flushBuffer();
}

BOOST_AUTO_TEST_CASE(testPush)
{
  auto monitoring = Monitoring::Get("stdout://");
  auto& qcMetric = monitoring->getAutoPushMetric("qcMetric");
  auto& qcMetric2 = monitoring->getAutoPushMetric("qcMetric2");
  std::this_thread::sleep_for (std::chrono::milliseconds(1500));
  qcMetric = 133 + 11 + 2.2;
  qcMetric2 = 133 - 11 - 2.2;
  std::this_thread::sleep_for (std::chrono::milliseconds(1500));
}

BOOST_AUTO_TEST_CASE(testSymbols)
{
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_APPMON ), "ApMon Backend disabled");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_LINUX ), "Linux OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_WINDOWS ), "Windows OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_MAC ), "Mac OS detected");
}

} // namespace Test
} // namespace monitoring
} // namespace o2
