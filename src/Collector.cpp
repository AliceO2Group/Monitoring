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

Collector::Collector()
{
	backends.emplace_back(new StdoutBackend());
	#ifdef _WITH_APPMON
	backends.emplace_back(new ApMonBackend(std::string("/home/awegrzyn/monitoring/pointer/config/apmon.conf")));
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

unsigned long Collector::getCurrentTimestamp()
{
        return std::chrono::duration_cast <std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
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

void Collector::insert(Metric* metric)
{
	auto search = cache.find(metric->getName());
	if (search == cache.end())
        {
        	cache.insert(std::pair<std::string, std::vector<Metric*>>(metric->getName(), std::vector<Metric*>()));
	}
	cache[metric->getName()].push_back(metric);
}

template<typename T>
void Collector::sendProcessed(T value, std::string name, std::string entity, unsigned long timestamp)
{
	Metric *metric = new TemplatedMetric<T>(value, name, entity, timestamp);
	for (auto const b: backends)
	{
		 metric->sendViaBackend(b);
	}
	insert(metric);
	injectRate(metric->getName());
}

template<typename T>
void Collector::send(T value, std::string name, std::string entity, unsigned long timestamp)
{
	for (auto const b: backends)
	{
		b->send(value, name, entity, timestamp);
	}
}

template void Collector::send(int, std::string, std::string, unsigned long);
template void Collector::send(double, std::string, std::string, unsigned long);
template void Collector::send(std::string, std::string, std::string, unsigned long);
template void Collector::send(uint32_t, std::string, std::string, unsigned long);

template void Collector::sendProcessed(int, std::string, std::string, unsigned long);
template void Collector::sendProcessed(double, std::string, std::string, unsigned long);
template void Collector::sendProcessed(std::string, std::string, std::string, unsigned long);
template void Collector::sendProcessed(uint32_t, std::string, std::string, unsigned long);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

