///
/// \file Zabbix.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_ZABBIX_H
#define ALICEO2_MONITORING_BACKENDS_ZABBIX_H

#include "Monitoring/Backend.h"
#include "../Transports/TCP.h"
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
class Zabbix final : public Backend
{
  public:
    /// Constructor, uses UDP transport
    /// \param hostname  InfluxDB UDP endpoint hostname
    /// \param port      InfluxDB UDP endpoint port number
    Zabbix(const std::string &hostname, int port);

    /// Default destructor
    ~Zabbix() = default;

    /// Sends metric to InfluxDB instance via one transport
    /// \param metric    reference to metric object
    void send(const Metric& metric) override;

    /// Adds tag
    /// \param name      tag name
    /// \param value     tag value
    void addGlobalTag(std::string name, std::string value) override;
  
  private:
    /// TCP transport
    std::unique_ptr<Transports::TCP> transport;

    /// Hostname as it's required by Zabbix protocol
    std::string hostname;

    /// Prepares Zabbix protocol message
    std::string metricToZabbix(const Metric& metric);

    /// Converts timestamp into unix format
    /// \param timestamp chrono system_clock timestamp
    /// \return unix timestamp
    std::string convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_BACKENDS_ZABBIX_H
