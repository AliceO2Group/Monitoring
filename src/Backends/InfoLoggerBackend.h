///
/// \file InfoLoggerBackend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H
#define ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H

#include "Monitoring/Backend.h"
//#include <InfoLogger/InfoLogger.hxx>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Backend that injects metrics to InfoLogger
///
/// InfoLogger does not support std::chrono::time_point therefore timestamps is converted to unsigned long
class InfoLoggerBackend final : public Backend
{
  public:
    /// Default constructor
    InfoLoggerBackend();
 
    /// Default destructor
    ~InfoLoggerBackend() = default;

    /// Sends metric to InfoLogger library
    /// \param metric           reference to metric object    
    void send(const Metric& metric) override;

    /// Sends multiple metrics not related to each other
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Sending multiple metrics is NOT supported by the InfoLogger therefore it falls back to sending metric one by one
    /// \param measurement   measurement name
    /// \param metrics  list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

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

    //AliceO2::InfoLogger::InfoLogger mInfoLogger;
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_INFOLOGGER_BACKEND_H
