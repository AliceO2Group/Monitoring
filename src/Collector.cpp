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

Collector::Collector(/*std::string configSource*/)
{
	//mConfigFile.load(configSource);
	//mConfigFile.getValue<string>("ExampleTask.moduleName");
	backends.emplace_back(new StdoutBackend());
	#ifdef _WITH_APPMON
	backends.emplace_back(new ApMonBackend(std::string("/home/awegrzyn/apmon.conf")));
	#endif
}

Collector::~Collector() {
	for (auto const vec: cache)
        {
                for (auto metric: vec.second)
		{
			delete (metric);
		}
        }
	for (auto const b: backends)
        {
		delete(b);
        }

}
std::chrono::time_point<std::chrono::system_clock> Collector::getCurrentTimestamp()
{
	return std::chrono::system_clock::now();
}

void Collector::addDerivedMetric(DerivedMetricMode mode, std::string name)
{
        registered.insert(std::pair<std::string, DerivedMetricMode>(name, mode));
	MonInfoLogger::GetInstance() << "Monitoring : Metric " << name << " added to derived metrics" << AliceO2::InfoLogger::InfoLogger::endm;
}

void Collector::injectRate(std::string name)
{
	auto search = cache.find(name);
        if (search != cache.end())
	{
		int size = search->second.size();
		if (size >= 2)
		{
			Metric* substract = search->second[size - 1]->substract(search->second[size - 2]);
			if (substract == nullptr) return;
			for (auto const b: backends)
			{
		                substract->sendViaBackend(b);
        		}
			delete(substract);
		}
	}
}

void Collector::injectAverage(std::string name)
{
	auto search = cache.find(name);
        if (search != cache.end())
        {
		Metric* average = search->second.back()->average(search->second);
		for (auto const b: backends)
		{	
			average->sendViaBackend(b);
		}
		delete(average);
	}
}
void Collector::insert(Metric* metric)
{
	auto search = cache.find(metric->getName());
	if (search == cache.end())
        {
        	cache.insert(std::pair<std::string, std::vector<Metric*>>(metric->getName(), std::vector<Metric*>()));
	}
	cache[metric->getName()].push_back(metric);
}
void Collector::processMetric(DerivedMetricMode mode, Metric* metric)
{
	insert(metric);
	if (mode == DerivedMetricMode::RATE)
	{
		injectRate(metric->getName());
	}
	else if (mode == DerivedMetricMode::AVERAGE)
	{
		injectAverage(metric->getName());
	}
	else 
	{
		MonInfoLogger::GetInstance() << "Monitoring : Processing mode incorrect for metric " << metric->getName() << AliceO2::InfoLogger::InfoLogger::endm;
	}
}

template<typename T>
void Collector::send(T value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	auto search = registered.find(name);
	if (search != registered.end()) processMetric(search->second, new TemplatedMetric<T>(value, name, entity, timestamp));
	
	for (auto const b: backends)
	{
		b->send(value, name, entity, timestamp);
	}
}

template void Collector::send(int, std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(double, std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(std::string, std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(uint32_t, std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

