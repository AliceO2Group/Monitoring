///
/// \file 13-PullClient.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main()
{
  /// List of topics to subscribe
  std::vector<std::string> topics = {"cru.link_status"};
  /// Connect to server
  auto client = MonitoringFactory::GetPullClient("adam-kafka:9092", topics);
  for (;;) {
    auto metrics = client->pull();
    if (!metrics.empty()) {
      for (auto& metric : metrics) {
        /// metric.first => topic name; metric.second => metric itself as string
        /// metric sample: link,serialId=983,endpoint=1,CRU=1,id=11,type=CRU pciAddress="b0:00.0",status=1i 1673940809860009855
        std::cout << metric.second << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
