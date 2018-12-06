///
/// \file 7-InternalBenchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

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
    monitoring->send(Metric{10, "myMetricInt"}.addTags({tags::Detector::TPC}));
    monitoring->send(Metric{10.10, "myMetricFloat"}.addTags({tags::Detector::TRD}));
  }
}

int main(int argc, char** argv) {
  auto monitoring = MonitoringFactory::Get(argv[1]);

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  test(monitoring);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  high_resolution_clock::time_point t3 = high_resolution_clock::now();
  test(monitoring);
  high_resolution_clock::time_point t4 = high_resolution_clock::now();

  auto durationTag = duration_cast<milliseconds>(t4 - t3).count();
  auto duration = duration_cast<milliseconds>(t2 - t1).count();

  std::cout << "Test 1M: " << duration << "us, Test 1M Tags: " << durationTag << "us" << std::endl;
}	
