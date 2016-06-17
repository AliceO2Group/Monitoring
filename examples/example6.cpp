#include <iostream>
#include "Monitoring/ProcessMonitor.h"

namespace Monitoring = AliceO2::Monitoring;

int main() {

	// create monitoring object and confuguration as parameter to constructor
	std::unique_ptr<Monitoring::Core::ProcessMonitor> monitor(new Monitoring::Core::ProcessMonitor());
	
	monitor->startMonitor();
}	
