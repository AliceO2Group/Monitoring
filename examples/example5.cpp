#include <iostream>
#include <chrono>
#include <thread>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
  // parse configuration file
  ConfigFile mConfigFile;
  mConfigFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini");
  // create monitoring object and confuguration as parameter to constructor
  std::unique_ptr<Monitoring::Core::Collector> monitoring(new Monitoring::Core::Collector(mConfigFile));

  auto start = std::chrono::system_clock::now();
	
  /// sample string, double uint32 values
  std::string testString("monitoring string");
  double testDouble = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
  uint32_t testUint32 = 555;
	
  // automaric derived metrics
  monitoring->addDerivedMetric(Monitoring::Core::DerivedMetricMode::RATE, std::string("DoubleMetric"));
  monitoring->addDerivedMetric(Monitoring::Core::DerivedMetricMode::AVERAGE, std::string("Uint32_tMetric"));
  for (int i = 0; i < 3; i++) {
    int testInt = std::rand();
    monitoring->send(testInt, std::string("IntMetric"));
    monitoring->send(testDouble, std::string("DoubleMetric"));
    monitoring->send(testString, std::string("StringMetric"));
    monitoring->send(testUint32, std::string("Uint32_tMetric"));

    testDouble += 10.0;
    testUint32 *= 2;
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
	
  // calculate execution time
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Execution (ms): " << elapsed.count() << '\n';
}
