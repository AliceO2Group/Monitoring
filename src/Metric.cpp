#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include "Monitoring/Metric.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

Metric::Metric(std::string&& _name, std::string&& _entity, unsigned long _timestamp) :
	timestamp ( _timestamp ),
	name ( _name ),
	entity ( _entity )
{}

std::string Metric::getName()
{
	return name;
}
unsigned long Metric::getTimestamp()
{
        return timestamp;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
