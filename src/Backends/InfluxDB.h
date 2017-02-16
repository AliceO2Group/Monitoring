///
/// \file InfluxDB.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
#define ALICEO2_MONITORING_BACKENDS_INFLUXDB_H

#include "Monitoring/Backend.h"
#include "../Transports/TransportInterface.h"
#include "../MonInfoLogger.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <curl/curl.h>
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
    /// Constructor, uses UDP transport
    /// \param hostname  InfluxDB UDP endpoint hostname
    /// \param port      InfluxDB UDP endpoint port number
    InfluxDB(const std::string &hostname, int port);

    /// Constructor, uses HTTP trasnport
    /// \param hostname  InfluxDB HTTP endpoint hostanme
    /// \param port      InfluxDB HTTP endpoint port number
    /// \param database  InfluxDB database name
    InfluxDB(const std::string &hostname, int port, const std::string& database);

    /// Default destructor
    ~InfluxDB() = default;

    /// Convert timestamp to unsigned long as required by InfluxDB
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
    std::unique_ptr<Transports::TransportInterface> transport;
    std::string tagSet; ///< Global tagset (common for each metric)

    /// Escapes " ", "," and "=" characters
    /// \param escaped   string rerference to escape characters from
    void escape(std::string& escaped);
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
