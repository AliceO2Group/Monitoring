///
/// \file ApMonBackend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_APMON_BACKEND_H
#define ALICEO2_MONITORING_CORE_APMON_BACKEND_H

#include <string>
#include <chrono>
#include <memory>
#include <ApMon.h>
#include "Monitoring/Backend.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// \brief Backend that uses AppMon (MonALISA)
///
/// Uses ApMon library to push values to MonALISA Service.
/// ApMon accepts timestamps as integers, therefore cast is needed (see method #convertTimestamp)
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class ApMonBackend final : public Backend
{
  public:
    /// Constructs AppMon backend
    /// \param configurationFile 	filepath to ApMon configuration file
    ApMonBackend(const std::string& configurationFile);

    /// Default destructor
    ~ApMonBackend() = default;

    void send(const Metric& metric) override;
    void addGlobalTag(std::string name, std::string value) override;
  private:
    /// Converts timestamp to format supported by ApMon
    /// \param timestamp 	timestamp in std::chrono::time_point format
    /// \return 		timestamp as integer (milliseconds from epoch)
    int convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);
  
    std::unique_ptr<ApMon> mApMon; ///< ApMon object
    std::string entity; ///< MonALISA entity
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H
