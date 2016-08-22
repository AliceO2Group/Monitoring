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

/// \brief Allows self monitoring or to monitor processes running at the same machine
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>

class ProcessMonitor {
public:
  /// Enables self monitoring
  /// \param collector	shared pointer to collector, it executes only thread-safe methods of Collector
  /// \param interval 	time interval between updates
  ProcessMonitor(std::shared_ptr<Collector> collector, int interval);
	
  /// Monitors single process
  /// \param collector    shared pointer to collector, it executes only thread-safe methods of Collector
  /// \param interval     time interval between updates
  /// \param pid 		PID number
  ProcessMonitor(std::shared_ptr<Collector> collector, int interval, int pid);

  /// Monitors multiple processes
  /// \param collector    shared pointer to collector, it executes only thread-safe methods of Collector
  /// \param interval     time interval between updates
  /// \param pids 	vector of PIDs
  ProcessMonitor(std::shared_ptr<Collector> collector, int interval, std::vector<int> pids);
	
  /// Joins parent thread if joinable
  ~ProcessMonitor();

  /// Launches new thread
  void startMonitor();
private:

  /// Pointer to Collector - used to sent values	
  std::shared_ptr<Collector> collector;
  
  /// Time-interval between updates (in seconds)
  const int interval;

  /// Vector of PIDs that will be monitored
  std::vector<int> pids;

  /// Executes terminal command
  std::string exec(const char* cmd);

  /// Vector of PID's parameters and values
  std::vector<std::string> getPIDStatus(int pid);

  /// options to be passed to PS
  std::string options;

  /// Main loop of monitoring thread
  void threadLoop();

  /// Thread object
  std::thread monitorThread;

  /// List params for PS                 int,    string, string, float,  float,  int,   int,   string
  const std::vector<std::string> labels {"pid", "etime", "time", "pcpu", "pmem", "rsz", "vsz", "comm"};
  const std::vector<std::pair<std::string, int>> params { {"pid", 0 }, {"etime", 2}, {"time", 2}, {"pcpu", 1 }, {"pmem", 1}, {"rsz", 0}, {"vsz", 0}, {"comm", 2} };

  /// parses above vector of strings into comma seperated string
  void preparePsOptions();
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSMONITOR_H

