///
/// \file InfluxDB.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
#define ALICEO2_MONITORING_BACKENDS_INFLUXDB_H

#include "Monitoring/Backend.h"
#include "../UriParser/UriParser.h"
#include "../Transports/TransportInterface.h"
#include "../MonLogger.h"
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

/// Backend that sends metrics to InfluxDB time-series databse
///
/// Metrics are converted into Influx Line protocol and then sent via one of available transports
class InfluxDB final : public Backend
{
  public:
    /// Constructor for both HTTP and  UDP transports
    /// \param hostname  InfluxDB UDP endpoint hostname
    /// \param port      InfluxDB UDP endpoint port number
    InfluxDB(const http::url& uri);

    /// Default destructor
    ~InfluxDB() = default;

    /// Convert timestamp to unsigned long as required by InfluxDB
    /// \param 		 chrono time_point timestamp
    /// \return  	 timestamp in nanoseconds
    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to InfluxDB instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Sends multiple values in single measurement
    /// \param name 	measurement name
    /// \param metrics 	list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string name, std::string value) override;
  
  private:
    std::unique_ptr<Transports::TransportInterface> transport;
    std::string tagSet; ///< Global tagset (common for each metric)

    /// Escapes " ", "," and "=" characters
    /// \param escaped   string rerference to escape characters from
    void escape(std::string& escaped);

    /// Modifies values to Influx Line Protocol format
    /// \param value 	reference to value
    /// \param type	type of the metric
    void prepareValue(std::string& value, int type);
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
