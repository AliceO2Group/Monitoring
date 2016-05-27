#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
       	
	// parse configuration file
	ConfigFile mConfigFile;
        mConfigFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini");

	// create monitoring object and confuguration as parameter to constructor
        std::unique_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector(mConfigFile));
	// derived metric :  rate
	collector->addDerivedMetric(Monitoring::Core::DerivedMetricMode::RATE, "myCrazyMetric1");

	// now send at least two metrics to see the result

	collector->send(10, "myCrazyMetric1");
	collector->send(20, "myCrazyMetric1");
	collector->send(30, "myCrazyMetric1");
	collector->send(40, "myCrazyMetric1");
}
