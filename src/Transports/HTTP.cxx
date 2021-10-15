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
/// \file HTTP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "HTTP.h"
#include "../MonLogger.h"
#include "../Exceptions/MonitoringException.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

HTTP::HTTP(const std::string& url)
{
  mHeaders = NULL;
  mCurl = curl_easy_init();
  curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(mCurl, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(mCurl, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(mCurl, CURLOPT_POST, 1);
  curl_easy_setopt(mCurl, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(mCurl, CURLOPT_TCP_KEEPINTVL, 60L);
  FILE *devnull = fopen("/dev/null", "w+");
  curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, devnull);

  MonLogger::Get(Severity::Info) << "HTTP transport initialized (" << url << ")" << MonLogger::End();
}

HTTP::~HTTP()
{
  curl_slist_free_all(mHeaders);
  curl_easy_cleanup(mCurl);
  curl_global_cleanup();
}

void HTTP::addHeader(const std::string& header)
{
  mHeaders = curl_slist_append(mHeaders, header.c_str());
  curl_easy_setopt(mCurl,  CURLOPT_HTTPHEADER, mHeaders);
}

void HTTP::send(std::string&& post)
{
  CURLcode response;
  long responseCode;
  curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(mCurl, CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(mCurl);
  curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    MonLogger::Get(Severity::Warn) << "HTTP Tranport " << curl_easy_strerror(response) << MonLogger::End();
  }
  if (responseCode < 200 || responseCode > 206) {
    MonLogger::Get(Severity::Warn) << "HTTP Transport: Response code : " << std::to_string(responseCode) << MonLogger::End();
  }
}

} // namespace transports
} // namespace monitoring
} // namespace o2
