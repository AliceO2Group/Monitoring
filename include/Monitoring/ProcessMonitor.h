#ifndef ALICEO2_MONITORING_CORE_PROCESSMONITOR_H
#define ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>

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
	ProcessMonitor();
	ProcessMonitor(int pid);
	ProcessMonitor(std::vector<int> pids);
	~ProcessMonitor();
	void startMonitor();
private:
	/// Vector of PIDs to monitor
	std::vector<int> pids;

	/// Executing terminal command
	std::string exec(const char* cmd);

	/// Vector of PIDs parameters
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

