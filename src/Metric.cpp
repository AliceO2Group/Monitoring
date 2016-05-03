#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include "Monitoring/Metric.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

Metric::Metric(std::string&& _name, std::string&& _entity, std::chrono::time_point<std::chrono::system_clock>&& _timestamp) :
	timestamp ( _timestamp ),
	name ( _name ),
	entity ( _entity )
{}

std::string Metric::getName()
{
	return name;
}
std::chrono::time_point<std::chrono::system_clock> Metric::getTimestamp()
{
        return timestamp;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
