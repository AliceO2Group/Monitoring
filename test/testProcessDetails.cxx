#include "../src/ProcessDetails.h"

#define BOOST_TEST_MODULE Test Monitoring ProcessDetails
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(createProcessDetails)
{
  AliceO2::Monitoring::ProcessDetails processDetails;
  auto pid = processDetails.getPid();
  BOOST_CHECK(pid > 1);
  processDetails.getProcessName();
  processDetails.getHostname();
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
