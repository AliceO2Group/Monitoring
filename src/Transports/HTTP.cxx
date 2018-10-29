///
/// \file HTTP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "HTTP.h"
#include "../MonLogger.h"
#include "../Exceptions/MonitoringException.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

HTTP::HTTP(const std::string& url) :
  curlHandle(initCurl(url), &HTTP::deleteCurl)
{
}

CURL* HTTP::initCurl(std::string url)
{
  CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_ALL);
  if (globalInitResult != CURLE_OK) {
    throw MonitoringException("cURL init", curl_easy_strerror(globalInitResult));
  }
  
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
  FILE *devnull = fopen("/dev/null", "w+");
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, devnull);
  return curl;
}

void HTTP::deleteCurl(CURL * curl)
{
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

void HTTP::send(std::string&& post)
{
  CURLcode response;	
  long responseCode;
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(curlHandle.get());
  curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    MonLogger::Get() << "HTTP Tranport " << curl_easy_strerror(response) << MonLogger::End();
  }
  if (responseCode < 200 || responseCode > 206) {
    MonLogger::Get() << "HTTP Transport: Response code : " << std::to_string(responseCode) << MonLogger::End();
  }
}

} // namespace transports
} // namespace monitoring
} // namespace o2
