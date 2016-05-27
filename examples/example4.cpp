#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
       	
	// create monitoring object
	// pass filepath to configuration as argument
        std::unique_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini"));

	
	// derived metric :  rate
	collector->addDerivedMetric(Monitoring::Core::DerivedMetricMode::RATE, "myCrazyMetric");

	// now send at least two metrics to see the result
	collector->send(10, "myCrazyMetric");
	collector->send(20, "myCrazyMetric");
	collector->send(30, "myCrazyMetric");
	collector->send(40, "myCrazyMetric");
}
