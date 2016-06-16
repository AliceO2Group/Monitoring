#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include "Monitoring/Metric.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

std::string Metric::getEntity()
{
        return entity;
}

std::chrono::time_point<std::chrono::system_clock> Metric::getTimestamp()
{
        return timestamp;
}

int Metric::getType()
{
        return value.which();
}

std::string Metric::getName()
{
        return name;
}

Metric::Metric(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
	value(value), name(name), entity(entity), timestamp(timestamp)
{}

Metric::Metric(std::string value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
	value(value), name(name), entity(entity), timestamp(timestamp)
{}

Metric::Metric(double value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
	value(value), name(name), entity(entity), timestamp(timestamp)
{}

Metric::Metric(uint32_t value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) :
	value(value), name(name), entity(entity), timestamp(timestamp)
{}

boost::variant< int, std::string, double, uint32_t > Metric::getValue()
{
        return value;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
