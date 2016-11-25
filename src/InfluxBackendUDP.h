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

/// Library backend that injects metrics to InfluxDB time-series databse
///
/// Metrics are transfered via cURL library.
/// InfluxDB does not support std::chrono::time_point therefore timestamps are converted to unsigned long (see #convertTimestamp)
class InfluxBackendUDP final : public Backend
{
  public:
    /// Constructor
    InfluxBackendUDP(const std::string &hostname, int port);

    /// Default destructor
    ~InfluxBackendUDP() = default;

    void sendUDP(std::string value, const std::string& name, const std::string& entity,
      const unsigned long timestamp);

    inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

        /// Pushes integer metric
    /// \param value        metric value (integer)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    void send(int value, const std::string& name, const std::string& entity, 
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) override;

    /// Pushes double metric
    /// \param value        metric value (double)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point
    void send(double value, const std::string& name, const std::string& entity, 
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) override;

    /// Pushes string metric
    /// \param value        metric value (string)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    void send(std::string value, const std::string& name, const std::string& entity,
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) override;

    /// Pushes uint32_t metric
    /// \param value        metric value (uint32_t)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    void send(uint32_t value, const std::string& name, const std::string& entity,
      const std::chrono::time_point<std::chrono::system_clock> &timestamp) override;

  private:
    boost::asio::io_service mIoService;
    boost::asio::ip::udp::socket mSocket;
    boost::asio::ip::udp::endpoint mEndpoint;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFLUXUDP_H
