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

/// Backend that injects metrics to InfluxDB time-series databse over UDP
///
/// Metrics are converted into Influx Line protocol and then sent 
/// via UDP using  boost asio library
class InfluxDB final : public Backend
{
  public:
    /// Constructor
    InfluxDB(const std::string &hostname, int port);
    InfluxDB(const std::string &hostname, int port, const std::string& database);

    /// Default destructor
    ~InfluxDB() = default;

    /// Convert timestamp to unsigned long as required by InfluxDB
    /// \param 		chrono time_point timestamp
    /// \return  	timestamp in nanoseconds
    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    /// Sends metric to InfluxDB
    //    /// \param metric           reference to metric object
    void send(const Metric& metric) override;

    /// Adds tag
    /// \param name         tag name
    /// \param value        tag value
    void addGlobalTag(std::string name, std::string value) override;
  
  private:
    std::unique_ptr<Transports::TransportInterface> transport;
    std::string tagSet; ///< Global tagset (common for each metric)

    /// Escapes " ", "," and "=" characters
    /// \param escaped      string rerference to escape characters from
    void escape(std::string& escaped);
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
