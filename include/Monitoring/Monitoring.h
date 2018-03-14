///
/// \file Monitoring.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_MONITORING_H
#define ALICEO2_MONITORING_MONITORING_H

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "Monitoring/Backend.h"
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/ProcessMonitor.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// Collects metrics from user and dispatches them to selected Monitoring backends. It also monitors the process itself.
///
/// Collects user-defined metrics (see Metric class) and pushes them through all selected backends (see Backend).
/// Calculates derived metrics, such as rate and average value (see #addDerivedMetric method).
/// Adds default tags to each metric: proces name, hostname.
/// Monitors the process itself - including memory, cpu usage and running time (see ProcessMonitor).
class Monitoring
{
  public:
    /// Disables copy constructor
    Monitoring & operator=(const Monitoring&) = delete;
    Monitoring(const Monitoring&) = delete;
  
    /// Initializes backends based on passed configuration.
    /// Instantiates derived metrics processor (see DerivedMetrics class) and process monitor (see ProcessMonitor).
    Monitoring();

    /// Creates backend and appends it to backend list
    void addBackend(std::unique_ptr<Backend> backend);

    /// Joins process monitor thread if possible
    ~Monitoring();

    /// Sends a metric to all avaliabes backends
    /// If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
    /// \param metric            r-value to metric object
    void send(Metric&& metric, DerivedMetricMode mode = DerivedMetricMode::NONE);

    /// Sends multiple not related to each other metrics
    /// \param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics);

    /// Sends multiple realated to each other metrics under a common  measurement name
    /// You can imagine it as a data point with multiple values
    /// If it's not supported by a backend it fallbacks into sending one by one
    /// \param name		measurement name
    /// \param metrics		list of metrics
    void sendGrouped(std::string name, std::vector<Metric>&& metrics);

    /// Enables process monitoring
    /// \param interval		refresh interval
    void enableProcessMonitoring(int interval = 5);

    /// Starts timing
    /// Sets a start timestamp and timeout
    /// \param name 		metric name
    /// \param timeout		timeout
    void startTimer(std::string name);

    /// Stops timing
    /// Sets stop timestamp, calculates delta and sends value
    /// \param name 		metric name
    void stopAndSendTimer(std::string name);

    /// Flushes metric buffer (this can also happen when buffer is full)
    void flushBuffer();

    /// Enables metric buffering
    /// \param size 		buffer size
    void enableBuffering(const unsigned int size);
  private:
    /// Derived metrics handler
    /// \see class DerivedMetrics
    std::unique_ptr<DerivedMetrics> mDerivedHandler;

    /// Vector of backends (where metrics are passed to)
    std::vector <std::unique_ptr<Backend>> mBackends;

    /// List of timers
    std::unordered_map <std::string, std::chrono::time_point<std::chrono::steady_clock>> mTimers;

    /// States whether Process Monitor thread is running
    std::atomic<bool> mMonitorRunning;

    /// Process Monitor thread  
    std::thread mMonitorThread;

    /// Process Monitor object that sends updates about the process itself
    std::unique_ptr<ProcessMonitor> mProcessMonitor;

    /// Process Monitor thread loop
    /// \param interval 	sleep time in seconds
    void processMonitorLoop(int interval);

    /// Metric buffer
    std::vector<Metric> mStorage;

    /// Flag stating whether metric buffering is enabled
    bool mBuffering;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_MONITORING_H
