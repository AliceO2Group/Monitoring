#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "Monitoring/DerivedMetrics.h"
#include "Monitoring/InfoLoggerBackend.h"
#include "Monitoring/TemplatedMetric.h"
#include "Monitoring/MonInfoLogger.h"

#ifdef _WITH_APPMON
#include "Monitoring/ApMonBackend.h"
#endif

namespace AliceO2 {
namespace Monitoring {
namespace Core {

DerivedMetrics::~DerivedMetrics()
{
	for (auto const vec: cache)
        {
                for (auto metric: vec.second)
                {
                        delete (metric);
                }
        }

}

void DerivedMetrics::registerMetric(DerivedMetricMode mode, std::string name)
{
        registered.insert(std::pair<std::string, DerivedMetricMode>(name, mode));
	MonInfoLogger::GetInstance() << "Monitoring : Metric " << name << " added to derived metrics" << AliceO2::InfoLogger::InfoLogger::endm;
}

bool DerivedMetrics::isRegistered(std::string name)
{
	auto search = registered.find(name);
        return (search != registered.end());
}

Metric* DerivedMetrics::calculateRate(std::string name)
{
	auto search = cache.find(name);
        if (search != cache.end())
	{
		int size = search->second.size();
		if (size >= 2)
		{
			Metric* subtract = search->second[size - 1]->subtract(search->second[size - 2]);
			return subtract;
		}
	}
	return nullptr;
}

Metric* DerivedMetrics::calculateAverage(std::string name)
{
	auto search = cache.find(name);
        if (search != cache.end())
        {
		Metric* average = search->second.back()->average(search->second);
		return average;
	}
	return nullptr;
}
void DerivedMetrics::insert(Metric* metric)
{
	auto search = cache.find(metric->getName());
	if (search == cache.end())
        {
        	cache.insert(std::pair<std::string, std::vector<Metric*>>(metric->getName(), std::vector<Metric*>()));
	}
	cache[metric->getName()].push_back(metric);
}
Metric* DerivedMetrics::processMetric(Metric* metric)
{
	insert(metric);
	auto search = registered.find(metric->getName());
	
	if (search->second == DerivedMetricMode::RATE)
	{
		return calculateRate(metric->getName());
	}
	else if (search->second == DerivedMetricMode::AVERAGE)
	{
		return calculateAverage(metric->getName());
	}
	else 
	{
		MonInfoLogger::GetInstance() << "Monitoring : Processing mode incorrect for metric " << metric->getName() << AliceO2::InfoLogger::InfoLogger::endm;
		return nullptr;
	}
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

