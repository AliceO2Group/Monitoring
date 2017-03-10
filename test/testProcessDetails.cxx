#include "../src/ProcessDetails.h"

#define BOOST_TEST_MODULE testProcessDetails
#include <boost/test/included/unit_test.hpp>


namespace AliceO2 {
namespace Monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(createProcessDetails)
{
  AliceO2::Monitoring::ProcessDetails processDetails;
  processDetails.getPid();
  processDetails.getProcessName();
  processDetails.getHostname();
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
