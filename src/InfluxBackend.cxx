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
  MonInfoLogger::GetInstance() << "InfluxDB backend enabled" << AliceO2::InfoLogger::InfoLogger::endm;
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
  
  return curl;
}

void InfluxBackend::deleteCurl(CURL * curl)
{
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

void InfluxBackend::send(int value, const std::string& name, const std::string& entity, 
                         const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

void InfluxBackend::send(double value, const std::string& name, const std::string& entity, 
                         const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

void InfluxBackend::send(std::string value, const std::string& name, const std::string& entity, 
                         const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  value.insert(value.begin(), '"');
  value.insert(value.end(), '"');
  curlWrite(value, name, entity, convertTimestamp(timestamp));
}

void InfluxBackend::send(uint32_t value, const std::string& name, const std::string& entity, 
                         const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

int InfluxBackend::curlWrite(std::string value, const std::string& name, const std::string& entity, 
                             const unsigned long timestamp)
{
  std::string escapedName = name;
  // escape space in name for InluxDB
  boost::replace_all(escapedName, " ", "\\ ");

  // preparing post data
  std::stringstream convert;
  convert << escapedName << ",entity=" << entity << " value=" << value << " " << timestamp;
  string post = convert.str();

  // send via curl
  CURLcode response;	
  long responseCode;
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(curlHandle.get());
  curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    MonInfoLogger::GetInstance() << "!!! InfluxDB : cURL error : " << (curl_easy_strerror(response)) 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    return 2;
  }
  if (responseCode != 204) {
    MonInfoLogger::GetInstance() << "!!! InfluxDB : cURL response code " + to_string(responseCode) 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    return 1;
  }
    MonInfoLogger::GetInstance() << "InfluxDB : metric " <<  name << ", code " << responseCode 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
  return 0;
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

