///
/// \file Kafka.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_KAFKA_H
#define ALICEO2_MONITORING_BACKENDS_KAFKA_H

#include "Monitoring/Backend.h"
#include "InfluxDB.h"
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

/// \brief Backend that sends metrics to Kafka over Influx Line protocol
class Kafka final : public Backend
{
  public:
    /// \param host      Kafka UDP endpoint hostname
    /// \param port      Kafka UDP endpoint port number
    Kafka(const std::string& host, unsigned int port, const std::string& topic = "test");

    /// Default destructor
    ~Kafka();

    /// Convert timestamp to unsigned long as required by Kafka
    /// \param timestamp  chrono time_point timestamp
    /// \return  	 timestamp in nanoseconds
    inline std::string convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to Kafka instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Sends multiple metrics not related to each other - NOT SUPPORTED
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Sends multiple values in single measurement - NOT SUPPORTED
    /// \param measurement measurement name
    /// \param metrics 	list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string_view name, std::string_view value) override;
  
  private:
    RdKafka::Producer *producer; ///< Kafka producer instance
    std::string tagSet; ///< Global tagset (common for each metric)
    InfluxDB mInfluxDB; ///< InfluxDB instance
    std::string mTopic; ///< Kafka topic
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_KAFKA_H
