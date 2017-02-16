///
/// \file InfoLoggerBackend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H
#define ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H

#include "Monitoring/Backend.h"
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

/// Backend that injects metrics to InfoLogger
///
/// InfoLogger does not support std::chrono::time_point therefore timestamps is converted to unsigned long (see #convertTimestamp)
class InfoLoggerBackend final : public Backend
{
  public:
    /// Default constructor
    InfoLoggerBackend();
 
    /// Default destructor
    ~InfoLoggerBackend() = default;

    /// Sends metric to InfluxDB
    /// \param metric           reference to metric object    
    void send(const Metric& metric) override;

    /// Adds tag
    /// \param name         tag name
    /// \param value        tag value
    void addGlobalTag(std::string name, std::string value) override;

  private:
    /// Converts timestamp to unsigned long (miliseconds from epoch)
    /// \param timestamp    timestamp in std::chrono::time_point format
    /// \return             timestamp as unsigned long (miliseconds from epoch)
    unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    std::string tagString; ///< Global tagset (common for each metric)
};

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H
