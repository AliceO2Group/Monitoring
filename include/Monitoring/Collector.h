///
/// \file Collector.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "Monitoring/Backend.h"
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/ProcessMonitor.h"


namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core 
{

/// Collect metrics and dispatches them to selected backends for further processing.
///
/// Collects metrics (see Metric class) and pushes them through all selected backends (see Backend)
/// Supports feature of calculating derived metrics, such as rate and average value (see #addDerivedMetric method).
/// Generates default entity as concatenated hostname and process ID.
/// Interacts with ProcessMonitor that monitors allows to monitor current and other processes running
/// at the same machine (see ProcessMonitor).
class Collector
{
  public:
    /// Disables copy constructor
    Collector & operator=(const Collector&) = delete;
    Collector(const Collector&) = delete;
  
    /// Initializes backends based on passed configuration.
    /// Instantiates derived metrics processor (see DerivedMetrics class) and process monitor (see ProcessMonitor).
    /// \param configPath 	path to configuration
    Collector(const std::string& configPath);

    /// Joins process monitor thread if possible
    ~Collector();

    /// Generates current timestamp in milliseconds
    /// \return 	timestamp as unsigned long
    static std::chrono::time_point<std::chrono::system_clock> getCurrentTimestamp();

    /// Overwrites default entity value
    /// \param entity 	new entity value
    void setEntity(std::string entity);

    /// Sends a metric to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param value 		metric value
    /// \param name 		metric name
    /// \param timestamp 	metric timestamp in miliseconds, if not provided getCurrentTimestamp provides current value
    template<typename T> 
    void send(T value, std::string name, 
            std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp());
  
    /// Adds metric to derived metric list - each time the metric arrives the derived metric is calculated and pushed to all backends
    /// Following processing modes are supported: DerivedMetricMode::RATE, DerivedMetricMode::AVERAGE
    /// \param name
    /// \param mode
    void addDerivedMetric(std::string name, DerivedMetricMode mode);

    /// Sends metric object to a backend
    /// \param metric	pointer to Metric
    template<typename T> void sendMetric(std::unique_ptr<Metric> metric, T);
	
    /// Same as send but skips derived metrics logic
    template<typename T>
    void sendRawValue(T value, std::string name, 
      std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp()) const;

    /// Forces updates of Process Monitor parameters
    void sendProcessMonitorValues();

    /// Adds PID to list of monitored processes
    /// \param pid 	process PID
    void addMonitoredPid(int pid);

  private:
    /// Derived metrics handler
    /// \see class DerivedMetrics
    std::unique_ptr<DerivedMetrics> mDerivedHandler;

    /// Vector of backends (where metrics are passed to)
    std::vector <std::unique_ptr<Backend>> mBackends;

    /// Entity value
    std::string mEntity;

    /// States whether Process Monitor thread is running
    std::atomic<bool> mMonitorRunning;

    /// Process Monitor thread  
    std::thread mMonitorThread;

    /// Process Monitor object
    /// If automatic updates are not enabled user can invoke #sendProcessMonitorValues method
    std::unique_ptr<ProcessMonitor> mProcessMonitor;

    /// Process Monitor thread loop
    /// \param interval 	sleep time in seconds
    void processMonitorLoop(int interval);

    /// Generates entity value as concatenated hostname and process id
    void setDefaultEntity();
};
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
