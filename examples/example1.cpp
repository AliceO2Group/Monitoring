#include <iostream>
#include <chrono>
#include <thread>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
        Monitoring::Core::Collector *monitoring = new Monitoring::Core::Collector("../config/apmon.conf");
        auto start = std::chrono::system_clock::now();
	
	/// sample string, double uint32 values
	std::string testString("monitoring string");
	double testDouble = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	uint32_t testUint32 = 555;
	
	// automaric derived metrics
        monitoring->addDerivedMetric(Monitoring::Core::DerivedMetricMode::RATE, std::string("Double Metric"));
	monitoring->addDerivedMetric(Monitoring::Core::DerivedMetricMode::AVERAGE, std::string("Uint32_t Metric"));
	for (int i = 0; i < 3; i++) {
                int testInt = std::rand();
		monitoring->send(testInt, std::string("Int Metric"));
                monitoring->send(testDouble, std::string("Double Metric"));
                monitoring->send(testString, std::string("String Metric"));
                monitoring->send(testUint32, std::string("Uint32_t Metric"));

		testDouble += 10.0;
		testUint32 *= 2;
		std::this_thread::sleep_for(std::chrono::seconds(2));
        }
	
	// calculate execution time
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Execution (ms): " << elapsed.count() << '\n';
        delete monitoring;
}
