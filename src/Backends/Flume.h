///
/// \file Flume.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_FLUME_H
#define ALICEO2_MONITORING_BACKENDS_FLUME_H

#include "Monitoring/Backend.h"
#include "../Transports/TransportInterface.h"
#include "../MonInfoLogger.h"
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

/// Backend that sends metrics to custum Apache Flume source
///
/// Metrics are serialized to JSON strings and send via UDP
class Flume final : public Backend
{
  public:

    /// Constructor, uses UDP transport
    /// \param hostname  Flume HTTP endpoint hostanme
    /// \param port      Flume HTTP endpoint port number
    Flume(const std::string &hostname, int port);

    /// Default destructor
    ~Flume() = default;

    // Convert timestamp to unsigned long as required by Flume
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
    /// UDP transport
    std::unique_ptr<Transports::TransportInterface> mTransport;

    /// Flume backend global header (for each metric)
    boost::property_tree::ptree globalHeader;

    /// Serializes metric object to JSON
    /// \param metric
    /// \return JSON serializes metric
    std::string metricToJson(const Metric& metric); 
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_FLUME_H
