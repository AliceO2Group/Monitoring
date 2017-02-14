///
/// \file InfluxBackendUDP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_INFLUXUDP_H
#define ALICEO2_MONITORING_CORE_INFLUXUDP_H

#include "Monitoring/Backend.h"
#include "MonInfoLogger.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

/// Backend that injects metrics to InfluxDB time-series databse over UDP
///
/// Metrics are converted into Influx Line protocol and then sent 
/// via UDP using  boost asio library
class InfluxBackendUDP final : public Backend
{
  public:
    /// Constructor
    InfluxBackendUDP(const std::string &hostname, int port);

    /// Default destructor
    ~InfluxBackendUDP() = default;

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
    /// Boost Asio I/O functionality
    boost::asio::io_service mIoService;

    /// UDP socket
    boost::asio::ip::udp::socket mSocket;

    /// UDP endpoint
    boost::asio::ip::udp::endpoint mEndpoint;

    std::string tagSet; ///< Global tagset (common for each metric)

    /// Escapes " ", "," and "=" characters
    /// \param escaped      string rerference to escape characters from
    void escape(std::string& escaped);

    /// Sends metric in InfluxDB Line Protocol format via UDP
    /// \param lineMessage   metrc in Influx Line Protocol format
    void sendUDP(std::string&& lineMessage);
};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFLUXUDP_H
