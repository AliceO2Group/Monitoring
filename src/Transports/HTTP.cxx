///
/// \file HTTP.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "HTTP.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring transports
namespace Transports
{

HTTP::HTTP(const std::string& url) :
  curlHandle(initCurl(url), &HTTP::deleteCurl)
{
}

CURL* HTTP::initCurl(std::string url)
{
  CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_ALL);
  if (globalInitResult != CURLE_OK) {
    throw std::runtime_error(curl_easy_strerror(globalInitResult));
  }
  
  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
  
  return curl;
}

void HTTP::deleteCurl(CURL * curl)
{
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

void HTTP::send(std::string&& post) throw(std::runtime_error)
{
  CURLcode response;	
  long responseCode;
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(curlHandle.get());
  curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    throw std::runtime_error(std::string("InfluxDB : cURL error : ") + curl_easy_strerror(response));
  }
  if (responseCode != 204) {
    throw std::runtime_error("InfluxDB : cURL response code " + (std::to_string(responseCode)));
  }
}

} // namespace Transports
} // namespace Monitoring
} // namespace AliceO2
