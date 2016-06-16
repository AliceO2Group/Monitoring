#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "Monitoring/Collector.h"
#include "Monitoring/InfoLoggerBackend.h"

#ifdef _WITH_APPMON
#include "Monitoring/ApMonBackend.h"
#endif

#ifdef _WITH_INFLUX
#include "Monitoring/InfluxBackend.h"
#endif

namespace AliceO2 {
namespace Monitoring {
namespace Core {

Collector::Collector(ConfigFile &mConfigFile)
{
	if (mConfigFile.getValue<int>("InfoLoggerBackend.enable") == 1)
		backends.emplace_back(std::unique_ptr<Backend>(new InfoLoggerBackend()));
	#ifdef _WITH_APPMON
	if (mConfigFile.getValue<int>("AppMon.enable") == 1)
		backends.emplace_back(std::unique_ptr<Backend>(new ApMonBackend(mConfigFile.getValue<string>("AppMon.pathToConfig"))));
	#endif
	#ifdef _WITH_INFLUX
	if (mConfigFile.getValue<int>("InfluxDB.enable") == 1)
	        backends.emplace_back(std::unique_ptr<Backend>(new InfluxBackend(mConfigFile.getValue<string>("InfluxDB.writeUrl"))));
        #endif

	setUniqueEntity();
}

void Collector::setUniqueEntity()
{
        char hostname[255];
        gethostname(hostname, 255);

        std::ostringstream format;
        format << hostname << "." << getpid();

        uniqueEntity = format.str();
}

void Collector::setEntity(std::string entity)
{
	uniqueEntity = entity;
}

std::chrono::time_point<std::chrono::system_clock> Collector::getCurrentTimestamp()
{
	return std::chrono::system_clock::now();
}

template<typename T>
void Collector::sendMetric(std::unique_ptr<Metric> &&metric, T type)
{
	for (auto& b: backends)
        {
                b->send(boost::get<T>(metric->getValue()), metric->getName(), metric->getEntity(), metric->getTimestamp());
        }
}

void Collector::addDerivedMetric(DerivedMetricMode mode, std::string name)
{
        derivedHandler.registerMetric(mode, name);
}

template<typename T>
void Collector::send(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	if (derivedHandler.isRegistered(name))
	{
		std::unique_ptr<Metric> derived = derivedHandler.processMetric(value, name, uniqueEntity, timestamp);
		if (derived != nullptr) sendMetric(std::move(derived), value);
	}

	for (auto& b: backends)
	{
		b->send(value, name, uniqueEntity, timestamp);
	}
}

template void Collector::send(int, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(double, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(uint32_t, std::string, std::chrono::time_point<std::chrono::system_clock>);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

