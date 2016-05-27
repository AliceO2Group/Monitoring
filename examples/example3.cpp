#include <iostream>
#include <chrono>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
	// parse configuration file
       	ConfigFile mConfigFile;
        mConfigFile.load("file:/home/awegrzyn/hackathon/Monitoring/examples/example.ini");
	
	// create monitoring object and confuguration as parameter to constructor
        std::unique_ptr<Monitoring::Core::Collector> collector(new Monitoring::Core::Collector(mConfigFile));

	// generate current timestamp
	std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
	
	// now send an application specific metric
	// 10 is the value 
	// myCrazyMetric is the name of the metric
	collector->send(10, "myCrazyMetric", timestamp); 
	collector->send(40, "myCrazyMetric", timestamp);
}
