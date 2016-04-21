#include <iostream>
#include <chrono>
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
        for (int i = 0; i < 10; i++) {
                int testInt = std::rand();
		c->send(testInt, name, entity);
                c->send(testDouble, std::string("Double Metric"), std::string("entity 1"));
                c->send(testString, std::string("String Metric"), std::string("entity 1"));
                c->send(testUint32, std::string("Uint32_t Metric"), std::string("entity 1"));
		
		c->sendProcessed(testInt, std::string("Int Metric Processed"), std::string("entity 1"));
                c->sendProcessed(testDouble, std::string("Double Metric Processed"), std::string("entity 1"));
                c->sendProcessed(testString, std::string("String Metric Processed"), std::string("entity 1"));
                c->sendProcessed(testUint32, std::string("Uint32_t Metric Processed"), std::string("entity 1")); 
        }

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Execution (ms): " << elapsed.count() << '\n';
        delete c;
}
