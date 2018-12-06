///
/// \file Flume.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_FLUME_H
#define ALICEO2_MONITORING_BACKENDS_FLUME_H

#include "Monitoring/Backend.h"
#include "../Transports/TransportInterface.h"
#include "../MonLogger.h"
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Backend that sends metrics to custum Apache Flume source
///
/// Metrics are serialized to JSON strings and send via UDP
class Flume final : public Backend
{
  public:

    /// Constructor, creates UDP transport
    /// \param host      Flume endpoint hostanme
    /// \param port      Flume endpoint port number
    Flume(const std::string& host, unsigned int port);

    /// Default destructor
    ~Flume() = default;

    /// Convert timestamp to unsigned long as required by Flume
    /// \param timestamp chrono time_point timestamp
    /// \return  	 timestamp in nanoseconds
    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to InfluxDB instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Sends multiple metrics not related to each other
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Sends multiple metric in single packet
    /// Not supported by the backend therefore it falls back to sending metric one by one
    /// \param measurement     measurement name
    /// \param metrics  list of metrics 
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string_view name, std::string_view value) override;
      
  private:
    /// UDP transport
    std::unique_ptr<transports::TransportInterface> mTransport;

    /// Flume backend global header (for each metric)
    boost::property_tree::ptree globalHeader;

    /// Serializes metric object to JSON
    /// \param metric
    /// \return JSON serializes metric
    std::string metricToJson(const Metric& metric);
    std::string metricsToJson(std::string measurement, std::vector<Metric>&& metrics);
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_FLUME_H
