#include <iostream>
#include <chrono>
#include <thread>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
        Monitoring::Core::Collector *c = new Monitoring::Core::Collector();
        auto start = std::chrono::system_clock::now();
	std::string testString("monitoring string");
	double testDouble = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	uint32_t testUint32 = 555;
	std::string name = std::string("Int Metric");
	std::string entity = std::string("entity 1");
        c->registerMetric(std::string("Double Metric"), Monitoring::Core::REGISTER_RATE);
	c->registerMetric(std::string("Uint32_t Metric"), Monitoring::Core::REGISTER_AVERAGE);
	for (int i = 0; i < 3; i++) {
                int testInt = std::rand();
		c->send(testInt, name, entity);
                c->send(testDouble, std::string("Double Metric"), std::string("entity 1"));
                c->send(testString, std::string("String Metric"), std::string("entity 1"));
                c->send(testUint32, std::string("Uint32_t Metric"), std::string("entity 1"));

		testDouble += 10.0;
		testUint32 *= 2;
		std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Execution (ms): " << elapsed.count() << '\n';
        delete c;
}
