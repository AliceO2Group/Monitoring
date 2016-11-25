///
/// \file InfluxBackendUDP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_INFLUXUDP_H
#define ALICEO2_MONITORING_CORE_INFLUXUDP_H

#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// Library backend that injects metrics to InfluxDB time-series databse
///
/// Metrics are transfered via cURL library.
/// InfluxDB does not support std::chrono::time_point therefore timestamps are converted to unsigned long (see #convertTimestamp)
class InfluxBackendUDP
{
  public:
    /// Constructor
    InfluxBackendUDP(boost::asio::io_service& io_service, const std::string &hostname, int port);

    /// Default destructor
    ~InfluxBackendUDP() = default;

  private:
    boost::asio::io_service &mIoService;
    boost::asio::ip::udp::socket mSocket;
    boost::asio::ip::udp::endpoint mEndpoint;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFLUXUDP_H
