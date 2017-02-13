///
/// \file InfluxBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxBackend.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

InfluxBackend::InfluxBackend(std::string url) : curlHandle(initCurl(url), &InfluxBackend::deleteCurl)
{
  MonInfoLogger::Info() << "InfluxDB backend enabled: " << url  << AliceO2::InfoLogger::InfoLogger::endm;
}

CURL* InfluxBackend::initCurl(std::string url)
{
  CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_ALL);
  if (globalInitResult != CURLE_OK) {
    throw std::logic_error(curl_easy_strerror(globalInitResult));
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

void InfluxBackend::deleteCurl(CURL * curl)
{
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

void InfluxBackend::addGlobalTag(std::string name, std::string value)
{
  escape(name); escape(value);
  if (!tagSet.empty()) tagSet += ",";
  tagSet += name + "=" + value;
}

void InfluxBackend::escape(std::string& escaped)
{
  boost::replace_all(escaped, ",", "\\,");
  boost::replace_all(escaped, "=", "\\=");
  boost::replace_all(escaped, " ", "\\ ");
}

void InfluxBackend::send(const Metric& metric)
{
  std::string value = boost::lexical_cast<std::string>(metric.getValue());
  if (metric.getType() == MetricType::STRING) {
    escape(value);
    value.insert(value.begin(), '"');
    value.insert(value.end(), '"');
  }
  std::string name = metric.getName();
  escape(name);  

  // preparing post data
  std::stringstream convert;
  convert << name << "," << tagSet << " value=" << value << " " << convertTimestamp(metric.getTimestamp());
  curlWrite(convert.str());
}

void InfluxBackend::curlWrite(std::string&& post)
{
  CURLcode response;	
  long responseCode;
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(curlHandle.get());
  curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    MonInfoLogger::Warning() << "!!! InfluxDB : cURL error : " << (curl_easy_strerror(response)) 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    return;
  }
  if (responseCode != 204) {
    MonInfoLogger::Warning() << "!!! InfluxDB : cURL response code " + std::to_string(responseCode) 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    return;
  }
  return;
}

inline unsigned long InfluxBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

