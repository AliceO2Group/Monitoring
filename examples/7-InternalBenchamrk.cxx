///
/// \file 7-InternalBenchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <iomanip>

using namespace o2::monitoring;
using namespace std::chrono;

void test(std::unique_ptr<Monitoring>& monitoring) {
  for (int i = 0; i < 100000; i++) {
    monitoring->send({10, "myMetricInt"});
    monitoring->send({10.10, "myMetricFloat"});
  }
}

void testWithTags(std::unique_ptr<Monitoring>& monitoring) {
  monitoring->addGlobalTag("benchmark", "yes");
  for (int i = 0; i < 100000; i++) {
    monitoring->send(Metric{10, "myMetricInt"}.addTags({{"tag1", "val1"}}));
    monitoring->send(Metric{10.10, "myMetricFloat"}.addTags({{"tag2", "val2"}}));
  }
}

int main(int argc, char** argv) {
  static constexpr std::array<std::string_view,4> backends = {
    "no-op://",
    "flume://localhost:1234",
    "influxdb-udp://localhost:1234",
    "stdout://"
  };
  std::cout << "| " << std::setw(30) << "Backend" << " |"
            << std::setw(10) << "no tags" << "   |"
            << std::setw(10) << " (2+1) tags" << "  |" << std::endl;
  std::cout << "+--------------------------------+-------------+-------------+" << std::endl;
  for(auto& backend : backends) {
    std::cout.setstate(std::ios_base::failbit);
    auto monitoring = MonitoringFactory::Get(std::string(backend));
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    test(monitoring);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    testWithTags(monitoring);
    high_resolution_clock::time_point t4 = high_resolution_clock::now();

    std::cout.clear(); 
    std::cout << "| " << std::setw(30) << backend << " |";
    std::cout << std::setw(10) << duration_cast<milliseconds>(t2 - t1).count() << "us |";
    std::cout << std::setw(10) << duration_cast<milliseconds>(t4 - t3).count() << "us |" << std::endl;
  }
}	
