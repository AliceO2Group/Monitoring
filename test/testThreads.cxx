#include "../include/Monitoring/MonitoringFactory.h"

#define BOOST_TEST_MODULE Test Monitoring Threads
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <thread>
#include <array>

namespace o2
{
namespace monitoring
{
namespace Test
{

using namespace o2::monitoring;

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  std::array<std::thread, 4> threads;
  for (auto &thread : threads) {
    thread = std::thread([] {
      for (int i = 0; i < 20; i++) {
        auto monitoring = MonitoringFactory::Get("stdout://");
        monitoring->addGlobalTag("name", "Readout");
        monitoring->addGlobalTag(tags::Key::Name, tags::Value::Readout);
        monitoring->send({1, "myCrazyMetricI"});
        monitoring->send({13.33, "myCrazyMetricS"});
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

} // namespace Test
} // namespace monitoring
} // namespace o2
