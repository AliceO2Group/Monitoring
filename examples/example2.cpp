#include <iostream>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {
       	
	// create monitoring object
	// pass filepath to ApMon configuration as argument
	Monitoring::Core::Collector *collector = new Monitoring::Core::Collector("../config/apmon.conf");

	// set user specific entity value, eg.:
	collector->setEntity("testEntity");

	// now send an application specific metric
	// 10 is the value
	// myCrazyMetric is the name of the metri
	collector->send(10, "myCrazyMetric");
}
