#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "Monitoring/Collector.h"
#include "Monitoring/StdoutBackend.h"
#include "Monitoring/TemplatedMetric.h"

#ifdef _WITH_APPMON
#include "Monitoring/ApMonBackend.h"
#endif

namespace AliceO2 {
namespace Monitoring {
namespace Core {

Collector::Collector(std::string configurationPath)
{
	//mConfigFile.load(configurationPath);

	backends.emplace_back(new StdoutBackend());
	#ifdef _WITH_APPMON
	//backends.emplace_back(new ApMonBackend(mConfigFile.getValue<string>("AppMon.pathToConfig")));
	backends.emplace_back(new ApMonBackend(configurationPath));
	#endif
	derivedHandler = new DerivedMetrics();
	setUniqueEntity();
}

Collector::~Collector() {
	for (auto const b: backends)
        {
		delete(b);
        }
	delete(derivedHandler);
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

void Collector::addDerivedMetric(DerivedMetricMode mode, std::string name)
{
        derivedHandler->registerMetric(mode, name);
}

void Collector::sendMetric(Metric* metric)
{
	for (auto const b: backends)
	{
        	metric->sendViaBackend(b);
        }
}

template<typename T>
void Collector::send(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	if (derivedHandler->isRegistered(name))
	{
		Metric* derived = derivedHandler->processMetric(new TemplatedMetric<T>(value, name, uniqueEntity, timestamp));
		if (derived != nullptr) sendMetric(derived);
		delete(derived);
	}

	for (auto const b: backends)
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

