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
/// Collects metrics and dispatches them to selected backends.
///
/// Collects metrics (see Metric class) and pushes them through all selected backends (see Backend).
/// Supports feature of calculating derived metrics, such as rate and average value (see #addDerivedMetric method).
/// Adds default tags to each metric: PID, proces name, hostname.
/// Monitors the process itself - including memory and cpu usage and running time (see ProcessMonitor).
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
    static Collector& getInstance();
    static void configure(const std::string& configPath);
    /// Joins process monitor thread if possible
    ~Collector();

    /// Sends a metric to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param value 		metric value
    /// \param name 		metric name
    template<typename T> 
    void send(T value, std::string name);

    /// Sends a metric to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param metric            r-value to metric object
    void send(Metric&& metric);

    /// Sends a metric with tagset to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param value            metric value
    /// \param name             metric name
    /// \param tags             vector of tags associated with metric
    template<typename T>
    void sendTagged(T value, std::string name, std::vector<Tag>&& tags);

    /// Sends a timed-metric to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param value            metric value
    /// \param name             metric name
    /// \param timestamp        metric timestamp in miliseconds, if not provided getCurrentTimestamp provides current value
    template<typename T>
    void sendTimed(T value, std::string name, std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Adds metric to derived metric list - each time the metric arrives the derived metric is calculated and pushed to all backends
    /// Following processing modes are supported: DerivedMetricMode::RATE, DerivedMetricMode::AVERAGE
    /// \param name             metric name
    /// \param mode             mode
    void addDerivedMetric(std::string name, DerivedMetricMode mode);

  private:
    static std::string configPath;
    /// Derived metrics handler
    /// \see class DerivedMetrics
    std::unique_ptr<DerivedMetrics> mDerivedHandler;

    /// Vector of backends (where metrics are passed to)
    std::vector <std::unique_ptr<Backend>> mBackends;

    /// States whether Process Monitor thread is running
    std::atomic<bool> mMonitorRunning;

    /// Process Monitor thread  
    std::thread mMonitorThread;

    /// Process Monitor object that sends updates about the process itself
    std::unique_ptr<ProcessMonitor> mProcessMonitor;

    /// Process Monitor thread loop
    /// \param interval 	sleep time in seconds
    void processMonitorLoop(int interval);

    /// Sets default tags that are applied to all metrics: PID, proces name, hostname
    void setDefaultTags();
};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
