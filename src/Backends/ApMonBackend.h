///
/// \file ApMonBackend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_APMONBACKEND_H
#define ALICEO2_MONITORING_BACKENDS_APMONBACKEND_H

#include "Monitoring/Backend.h"
#include <ApMon.h>
#include <string>
#include <chrono>
#include <memory>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Backend that uses AppMon (MonALISA)
///
/// Uses ApMonBackend library to push metric to MonALISA Service.
/// ApMonBackend accepts timestamps as integers, therefore a cast is needed.
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class ApMonBackend final : public Backend
{
  public:
    /// Constructs AppMon backend
    /// \param path 	filepath to ApMonBackend configuration file
    ApMonBackend(const std::string& path);

    /// Default destructor
    ~ApMonBackend() = default;

    /// Sends multiple metrics not related to each other
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Sends metric via MonALISA
    /// ApMonBackend's intances is type-aware therefore cast of metric value is needed
    /// \param metric           reference to metric object:
    void send(const Metric& metric) override;

    /// Sends grouped metrics  under common measuremet name
    /// \param measurement     measurement name
    /// \param metrics  list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Extends entity value
    /// \param name             tag name (unused)
    /// \param value            tag value that is concatenated to entity string
    void addGlobalTag(std::string name, std::string value) override;
  private:
    /// Converts timestamp to format supported by ApMonBackend
    /// \param timestamp 	timestamp in std::chrono::time_point format
    /// \return 		timestamp as integer (milliseconds from epoch)
    int convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);
  
    std::unique_ptr<ApMon> mApMon; ///< ApMon object
    std::string entity; ///< MonALISA entity, created out of global tags
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_APMONBACKEND_H
