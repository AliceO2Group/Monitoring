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
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// Library backend that injects metrics to InfluxDB time-series databse over UDP
///
/// Metrics are sent over UDP via boost asio library
class InfluxBackendUDP final : public Backend
{
  public:
    /// Constructor
    InfluxBackendUDP(const std::string &hostname, int port);

    /// Default destructor
    ~InfluxBackendUDP() = default;

    /// Convert to InfluxDB Line Protocol and send via UDP (boost asio)
    /// \param value 	value already converted to string
    /// \param name         metric name
    /// \param timestamp    metric timestamp in nanoseconds
    void sendUDP(std::string value, std::string name,
      const unsigned long timestamp);

    /// Convert timestamp to unsigned long as required by InfluxDB
    /// \param 		chrono time_point timestamp
    /// \return  	timestamp in nanoseconds
    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    void send(const Metric& metric) override;
    void addGlobalTag(std::string name, std::string value) override;
    std::string tagSet;
  private:
    /// Boost Asio I/O functionality
    boost::asio::io_service mIoService;

    /// UDP socket
    boost::asio::ip::udp::socket mSocket;

    /// UDP endpoint
    boost::asio::ip::udp::endpoint mEndpoint;

    void escape(std::string& escaped);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFLUXUDP_H
