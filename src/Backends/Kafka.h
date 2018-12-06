///
/// \file Kafka.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_KAFKA_H
#define ALICEO2_MONITORING_BACKENDS_KAFKA_H

#include "Monitoring/Backend.h"
#include "../MonLogger.h"
#include <chrono>
#include <string>
#include <unordered_map>
#include <librdkafka/rdkafkacpp.h>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Backend that sends metrics to Kafka time-series databse
///
/// Metrics are converted into Influx Line protocol and then sent via one of available transports
class Kafka final : public Backend
{
  public:
    /// Constructor for UDP transport
    /// \param host      Kafka UDP endpoint hostname
    /// \param port      Kafka UDP endpoint port number
    Kafka(const std::string& host, unsigned int port);

    /// Default destructor
    ~Kafka();

    /// Convert timestamp to unsigned long as required by Kafka
    /// \param timestamp  chrono time_point timestamp
    /// \return  	 timestamp in nanoseconds
    inline std::string convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to Kafka instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Sends multiple metrics not related to each other
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Sends multiple values in single measurement
    /// \param measurement measurement name
    /// \param metrics 	list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string name, std::string value) override;
  
  private:
    RdKafka::Producer *producer;
    std::unordered_map<std::string, std::string> globalHeader;
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_KAFKA_H
