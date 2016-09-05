#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <tuple>
#include <atomic>
#include <thread>
#include "Configuration/Configuration.h"
#include "Monitoring/Backend.h"
#include "Monitoring/Metric.h"
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/ProcessMonitor.h"


namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {


/// \brief Collects metrics - public interface
///
/// Collects metrics and pushes them through all avaliable backends. Supported types: int, double, string, uint32_t.
/// Supports feature of calculating derived metrics, such as rate and average value (see #addDerivedMetric method).
/// Generates default entity (origin) as concatanated hostname and process ID.
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class Collector {

  /// Disable copy constructor
  Collector & operator=(const Collector&) = delete;
  Collector(const Collector&) = delete;

private:

  /// Object responsible from derived metrics
  /// \see class DerivedMetrics
  DerivedMetrics mDerivedHandler;

  /// Vector of backends (where the values are send to).
  std::vector <std::unique_ptr<Backend>> mBackends;
    
  /// Default entity value, see setUniqueEntity method
  std::string mUniqueEntity;

  /// States whether Process Monitor thread should run or join
  std::atomic<bool> mMonitorRunning;

  /// Process Monitor thread  
  std::thread monitorThread;

  /// Process Monitor object
  /// If automatic updates are not enabled still "monitorUpdate" method can be used
  std::unique_ptr<ProcessMonitor> mProcessMonitor;

  /// Process Monitor loop (of new thread)
  void processMonitorLoop(int interval);

  /// Generates entity value as concatenated hostname and process id
  void setUniqueEntity();

public:

  /// Initialaze backends and instance of "derived metric processor" (DerivedMetrics class)
  /// \param configFile 	configuration object
  Collector(ConfigFile &configFile);

  /// Wait and join monitoring thread
  ~Collector();

  /// Generates timestamp in miliseconds
  /// \return timestamp as unsigned long
  static std::chrono::time_point<std::chrono::system_clock> getCurrentTimestamp();

  /// Overwrites default entity value
  /// \param entity 	new entity value
  void setEntity(std::string entity);

  /// Sends the metric to all avaliabes backends
  /// If metric has been added to "derived metric processor" the derived metric is calculated (see addDerivedMetric method)
  /// \param value of the metric
  /// \param name of the metric
  /// \param timestamp in miliseconds, if not provided output of getCurrentTimestamp as default value is assigned
  template<typename T> 
  void send(T value, std::string name, 
            std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp());
  
  /// Adds metric to the list - each time the metric arrives the derived metric is calculated and then sent to all backends
  /// Following processing modes are supported: DerivedMetricMode::RATE, DerivedMetricMode::AVERAGE
  /// \param mode
  /// \param name
  void addDerivedMetric(DerivedMetricMode mode, std::string name);

  /// Sends Metric object to backend
  /// \param metric	 r-value pointer to Metric
  template<typename T> void sendMetric(std::unique_ptr<Metric> &&metric, T);
	
  /// Same as send but totally skips derived metrics logic
  template<typename T>
  void sendDirect(T value, std::string name, 
                  std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp()) const;

  /// Manual update and send of Process Monitor parameters
  void monitorUpdate();

  /// Add process PID that will be automatically monitored
  /// \param pid
  void addMonitoredPid(int pid);

};
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
