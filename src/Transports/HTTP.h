///
/// \file HTTP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_HTTP_H
#define ALICEO2_MONITORING_TRANSPORTS_HTTP_H

#include "TransportInterface.h"

#include <curl/curl.h>
#include <memory>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring transports
namespace Transports
{

/// HTTP POST transport for Monitoring backends
///
/// Allows to push string formatted metrics as HTTP POST requests via cURL
class HTTP : public TransportInterface
{
  public:
    /// Constructor
    /// \param url    URL of HTTP server endpoint
    HTTP(const std::string& url);

    /// Default destructor
    ~HTTP() = default;

    /// Sends metric to InfluxDB
    /// \param metric           reference to metric object	
    void send(std::string&& post) throw(std::runtime_error);
    
  private:
    /// Custom deleter of CURL object
    /// \param curl 	    CURL handle
    static void deleteCurl(CURL * curl);

    /// Initilizes CURL and all common options
    /// \param url 	    URL to InfluxDB
    /// \return             CURL handle
    CURL* initCurl(std::string url);
  
    /// CURL smart pointer with custom deleter
    std::unique_ptr<CURL, decltype(&HTTP::deleteCurl)> curlHandle;
};

} // namespace Transports
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_TRANSPORTS_HTTP_H
