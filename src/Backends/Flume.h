///
/// \file Flume.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_FLUME_H
#define ALICEO2_MONITORING_BACKENDS_FLUME_H

#include "Monitoring/Backend.h"
#include "../Transports/TransportInterface.h"
#include "../MonInfoLogger.h"
#include <atomic>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

/// Backend that sends metrics to InfluxDB time-series databse
///
/// Metrics are converted into Influx Line protocol and then sent via one of available transports
class Flume final : public Backend
{
  public:

    /// Constructor, uses HTTP trasnport
    /// \param hostname  InfluxDB HTTP endpoint hostanme
    /// \param port      InfluxDB HTTP endpoint port number
    Flume(const std::string &hostname, int port);

    /// Default destructor
    ~Flume();

    // Convert timestamp to unsigned long as required by InfluxDB
    /// \param 		 chrono time_point timestamp
    /// \return  	 timestamp in nanoseconds
    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to InfluxDB instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string name, std::string value) override;
      
  private:
    /// HTTP transport
    std::unique_ptr<Transports::TransportInterface> mTransport;

    /// Flume backend global header (for each metric)
    boost::property_tree::ptree globalHeader;

    /// New thread for dispatch metrics
    std::thread mDispatchThread;

    /// Thread loop that dispatches metric periodically (to limit number of HTTP requests)
    void dispatchLoop();

    /// Vector that stores metrics to be sent
    std::vector<Metric> mQueue;

    /// Mutex on queue vector
    std::mutex mQueueMutex;

    /// States whether dispatch thread should run or not
    std::atomic<bool> mThreadRunning;
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_FLUME_H
