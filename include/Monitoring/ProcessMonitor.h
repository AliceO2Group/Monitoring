#ifndef ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
#define ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "Collector.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

/// \brief Allows to monitor processes running at the same machine
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>

class ProcessMonitor {
public:
	/// Enables self monitoring
	/// \param collector	shared pointer to collector, it executes only thread-safe methods of Collector
	ProcessMonitor(std::shared_ptr<Collector> collector);
	
	/// Monitors single process
	/// \param collector    shared pointer to collector, it executes only thread-safe methods of Collector
	/// \param pid 		PID number
	ProcessMonitor(std::shared_ptr<Collector> collector, int pid);

	/// Monitors multiple processes
	/// \param collector    shared pointer to collector, it executes only thread-safe methods of Collector
	/// \param pids 	vector of PIDs
	ProcessMonitor(std::shared_ptr<Collector> collector, std::vector<int> pids);
	
	/// Joins parent thread if joinable
	~ProcessMonitor();

	/// Launches new thread
	void startMonitor();
private:
	std::shared_ptr<Collector> collector;

	/// Vector of PIDs that will be monitored
	std::vector<int> pids;

	/// Executes terminal command
	std::string exec(const char* cmd);

	/// Vector of PID's parameters and values
	std::vector<std::string> getPIDStatus(int pid);

	/// Main loop of monitoring thread
	void threadLoop();

	/// Thread object
	std::thread monitorThread;

	/// List params
	const std::vector<std::string> labels {"pid", "etime", "time", "pcpu", "pmem", "rsz", "vsz", "comm"};
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

