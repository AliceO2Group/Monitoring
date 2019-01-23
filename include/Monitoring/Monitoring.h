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
#include <deque>

#include "Monitoring/ComplexMetric.h"
#include "Monitoring/Backend.h"
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/ProcessMonitor.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Main class that collects metrics from user and dispatches them to selected monitoring backends.
///
/// Collects user-defined metrics (seeMetric class) and pushes them through all selected backends (seeBackend).
/// Calculates derived metrics such as rate and increment value (seeDerivedMetrics class).
/// Adds default tags to each metric: proces name, hostname (seeProcessDetails class)
/// Monitors the process itself - including memory, cpu and network usage (seeProcessMonitor class).
class Monitoring
{
  public:
    /// Disable copy constructor
    Monitoring & operator=(const Monitoring&) = delete;

    /// Disable copy constructor
    Monitoring(const Monitoring&) = delete;
  
    /// Instantiates derived metrics processor (see DerivedMetrics) and process monitor (seeProcessMonitor).
    Monitoring();

    /// Creates and appends backend to the backend list
    void addBackend(std::unique_ptr<Backend> backend);

    /// Joins process monitor thread if possible
    ~Monitoring();

    /// Sends a metric to all avaliabes backends
    /// If DerivedMetricMode is specified it generates and sends derived metric
    /// \param metric           r-value to metric object
    /// \param mode		Derived metric mode
    void send(Metric&& metric, DerivedMetricMode mode = DerivedMetricMode::NONE);

    /// Sends multiple realated to each other metric values under a common measurement name
    /// You can imagine it as a data point with multiple values
    /// If it's not supported by a backend it fallbacks into sending one by one
    /// \param name		measurement name
    /// \param metrics		list of metrics
    void sendGrouped(std::string name, std::vector<Metric>&& metrics, Verbosity verbosity = Metric::DEFAULT_VERBOSITY);

    /// Enables process monitoring
    /// \param interval		refresh interval
    void enableProcessMonitoring(const unsigned int interval = 5);

    /// Flushes metric buffer (this can also happen when buffer is full)
    void flushBuffer();

    /// Enables metric buffering
    /// \param size 		buffer size
    void enableBuffering(const std::size_t size = 128);

    /// Adds global tag
    /// \param name 		tag name
    /// \param value 		tag value
    void addGlobalTag(std::string_view name, std::string_view value);

    /// Adds predefined global tag
    /// \param tag		tag index (use predefined enums form tag:: namespace)
    void addGlobalTag(const unsigned int tag);

    /// Returns a metric which will be periodically sent to backends
    /// \param name 		metric name
    /// \return 		periodically send metric
    ComplexMetric& getAutoPushMetric(std::string name, unsigned int interval = 1);

  private:
    /// Sends multiple (not related to each other) metrics
    /// \param metrics  vector of metrics
    void transmit(std::vector<Metric>&& metrics);

    /// Flush buffer of desired verbosity
    void flushBuffer(short index);

    /// Matches verbosity of a backend and a metric in order to decide whether metric should be send to the backend
    bool matchVerbosity(Verbosity backend, Verbosity metric);

    /// Derived metrics handler
    /// \see class DerivedMetrics
    std::unique_ptr<DerivedMetrics> mDerivedHandler;

    /// Vector of backends (where metrics are passed to)
    std::vector <std::unique_ptr<Backend>> mBackends;

    /// Pushes metric to all backends or to the buffer
    void transmit(Metric&& metric);

    /// States whether Process Monitor thread is running
    std::atomic<bool> mMonitorRunning;

    /// Process Monitor thread  
    std::thread mMonitorThread;

    /// Process Monitor object that sends updates about the process itself
    std::unique_ptr<ProcessMonitor> mProcessMonitor;

    /// Push metric loop
    void pushLoop();

    /// Metric buffer
    std::unordered_map<std::underlying_type<Verbosity>::type, std::vector<Metric>> mStorage;

    /// Flag stating whether metric buffering is enabled
    bool mBuffering;

    /// Size of buffer
    std::size_t mBufferSize;

    /// Store for automatically pushed metrics
    std::deque<ComplexMetric> mPushStore;

    /// Process monitor interval
    std::atomic<unsigned int> mProcessMonitoringInterval;

    /// Automatic metric push interval
    std::atomic<unsigned int> mAutoPushInterval;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_MONITORING_H
