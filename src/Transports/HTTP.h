// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file HTTP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_HTTP_H
#define ALICEO2_MONITORING_TRANSPORTS_HTTP_H

#include "TransportInterface.h"

#include <curl/curl.h>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief HTTP POST transport
///
/// Allows to push string formatted metrics as HTTP POST requests via cURL
class HTTP : public TransportInterface
{
  public:
    /// Constructor
    /// \param url          URL of HTTP server endpoint
    HTTP(const std::string& url);

    /// Destructor
    ~HTTP();

    /// Sends metric via HTTP POST
    /// \param post       r-value reference string formatted metric
    void send(std::string&& post);

    /// Adds custom HTTP header
    void addHeader(const std::string& header);
  private:
    /// CURL pointers
    CURL *mCurl;

    /// HTTP headers struct
    struct curl_slist *mHeaders;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_HTTP_H
