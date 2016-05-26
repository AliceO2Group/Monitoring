#include <iostream>
#include <curl/curl.h>
#include "Monitoring/MonInfoLogger.h"
#include "Monitoring/InfluxBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {


inline unsigned long InfluxBackend::convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	return std::chrono::duration_cast <std::chrono::nanoseconds>(
		timestamp.time_since_epoch()
        ).count();
}
InfluxBackend::InfluxBackend()
{
	url = "http://pcald03.cern.ch:8086/write?db=o2";
}

void InfluxBackend::send(const int value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}
void InfluxBackend::send(const double value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}
void InfluxBackend::send(const std::string value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	curlWrite(value, name, entity, convertTimestamp(timestamp));
}
void InfluxBackend::send(const uint32_t value, const std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	curlWrite(std::to_string(value), name, entity, convertTimestamp(timestamp));
}

int InfluxBackend::curlWrite(const std::string value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	/// preparing post data
	stringstream convert;
        convert << name << ",entity=" << entity << " value=" << value << " " << timestamp;
        string post = convert.str();
	
	CURL *curl;
        CURLcode response;
        curl = curl_easy_init();
        long responseCode;
        if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
                curl_easy_setopt(curl, CURLOPT_POST, 1);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) post.length());
                response = curl_easy_perform(curl);
                curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &responseCode);
                if (response != CURLE_OK) {
                        std::cout << (curl_easy_strerror(response)) << std::endl;
			return 2;
                }
                if (responseCode != 204) {
                        std::cout << " \\!!! cURL response code " + to_string(responseCode) << std::endl;
			return 1;
                }
                curl_easy_cleanup(curl);
		MonInfoLogger::GetInstance() << "cURL : metric " <<  name << ", code " << responseCode << AliceO2::InfoLogger::InfoLogger::endm;
        }
        return 0;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

