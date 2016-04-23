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

unsigned long Collector::getCurrentTimestamp()
{
        return std::chrono::duration_cast <std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();
}

void Collector::registerMetric(std::string name, int mode)
{
        registered.insert(std::pair<std::string, int>(name, mode));
	MonInfoLogger::GetInstance() << "Monitoring : Metric " << name << " register to be processed in mode " << mode << AliceO2::InfoLogger::InfoLogger::endm;
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
		for (auto metric : search->second)
		{
			std::cout << metric->getName() << std::endl;
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
void Collector::processMetric(int mode, Metric* metric)
{
	insert(metric);
	if (mode == REGISTER_RATE)
	{
		injectRate(metric->getName());
	}
	else if (mode == REGISTER_AVERAGE)
	{
		injectAverage(metric->getName());
	}
	else 
	{
		MonInfoLogger::GetInstance() << "Monitoring : Processing mode incorrect for metric " << metric->getName() << AliceO2::InfoLogger::InfoLogger::endm;
	}
}

template<typename T>
void Collector::send(T value, std::string name, std::string entity, unsigned long timestamp)
{
	auto search = registered.find(name);
	if (search != registered.end()) processMetric(search->second, new TemplatedMetric<T>(value, name, entity, timestamp));
	
	for (auto const b: backends)
	{
		b->send(value, name, entity, timestamp);
	}
}

template void Collector::send(int, std::string, std::string, unsigned long);
template void Collector::send(double, std::string, std::string, unsigned long);
template void Collector::send(std::string, std::string, std::string, unsigned long);
template void Collector::send(uint32_t, std::string, std::string, unsigned long);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

