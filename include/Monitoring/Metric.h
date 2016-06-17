#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include "boost/variant.hpp"
#include "Monitoring/Backend.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

/// \brief Represents metric parameters except value itself (timestamp, name, entity)
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class Metric {
protected:
	/// Metric value
	const boost::variant< int, std::string, double, uint32_t > value;
	
	/// Metric name
	const std::string name;

	/// Metric entity (origin)
	const std::string entity;

	// Metric timestamp
        const std::chrono::time_point<std::chrono::system_clock> timestamp;
        
public:

	/// Initialize class variables : supporting int, double, uint32_t and std::string
	/// \param value of the metric
	/// \param name of the metric
	/// \param timestamp in miliseconds, if not provided output of getCurrentTimestamp as default value is assigned
	Metric(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
        Metric(std::string value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
	Metric(double value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
	Metric(uint32_t value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
	
	/// Default destructor
	~Metric() = default;
        
	/// Name getter
	/// \return	metric name
	std::string getName();

	/// Timestamp getter
	/// \return 	metric timestamp
        std::chrono::time_point<std::chrono::system_clock> getTimestamp();
	
	/// Value getter
	/// \return metric value
	boost::variant< int, std::string, double, uint32_t > getValue();

	/// Entity getter
	/// \return metric entity
	std::string getEntity();

	/// Value type getter
	// \return type of value stores inside metric : 0 - int, 1 - std::string, 2 - double, 3 - uint32_t
	int getType();
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
