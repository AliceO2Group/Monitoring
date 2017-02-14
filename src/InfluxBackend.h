///
/// \file InfluxBackend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_INFLUX_H
#define ALICEO2_MONITORING_CORE_INFLUX_H

#include "Monitoring/Backend.h"
#include <curl/curl.h>
#include <memory>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

/// Library backend that injects metrics to InfluxDB time-series databse
///
/// Metrics are transfered via cURL library.
/// InfluxDB does not support std::chrono::time_point therefore timestamps are converted to unsigned long (see #convertTimestamp)
class InfluxBackend final : public Backend
{
  public:
    /// Constructor
    InfluxBackend(std::string url);

    /// Default destructor
    ~InfluxBackend() = default;

    /// Sends metric to InfluxDB
    /// \param metric           reference to metric object	
    void send(const Metric& metric) override;
    
    /// Adds tag
    /// \param name         tag name
    /// \param value        tag value
    void addGlobalTag(std::string name, std::string value) override;

  private:
    /// Custom deleter of CURL object
    /// \param curl 	    CURL handle
    static void deleteCurl(CURL * curl);

    /// Initilizes CURL and all common options
    /// \param url 	    URL to InfluxDB
    /// \return             CURL handle
    CURL* initCurl(std::string url);
  
    /// CURL smart pointer with custom deleter
    std::unique_ptr<CURL, decltype(&InfluxBackend::deleteCurl)> curlHandle;

    /// Converts timestamp to unsigned long (nanoseconds from epoch)
    /// \param timestamp    timestamp in std::chrono::time_point format
    /// \return             timestamp as unsigned long (nanoseconds from epoch)
    unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);
	
    /// Sends metric via HTTP POST
    /// \param post         metric in Influx Line Protocol format
    void curlWrite(std::string&& post);
 
    std::string tagSet; ///< Global tagset (common for each metric)

    /// Escapes " ", "," and "=" characters
    /// \param escaped      string rerference to escape characters from
    void escape(std::string& escaped);

};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_INFLUX_H
