#include <iostream>
#include <regex>
#include <string>
#include <vector>

#define BOOST_TEST_MODULE Test Monitoring RegexMatch
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace o2 {
namespace monitoring {
namespace Test {

BOOST_AUTO_TEST_CASE(retrieveOtherParams)
{
  static std::regex metricsRE(R"regex(\[METRIC\] ([a-zA-Z0-9/_-]+),(0|1|2|4) ([0-9.a-zA-Z_/" <>()<$:]+) ([0-9]+))regex", std::regex::optimize);

  std::vector<std::string> metrics = {
    "[METRIC] bkey,0 12 1789372894 hostname=test.cern.ch",
    "[METRIC] bkey,0 \"Sample string\" 1789372894 hostname=test.cern.ch",
    "[METRIC] bkey,0 \"<matcher query: (and origin:TST (and description:A1 (and subSpec:77 (just startTime:$0 ))))>\" 1789372894 hostname=test.cern.ch"
  };

  std::smatch match;
  for (const auto& metric : metrics) {
    BOOST_CHECK(std::regex_search(metric, match, metricsRE));
  }
}

} // namespace Test
} // namespace monitoring
} // namespace o2
