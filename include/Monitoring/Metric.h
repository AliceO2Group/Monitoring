#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <vector>
#include <chrono>
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

	/// Metric timestamp
        const std::chrono::time_point<std::chrono::system_clock> timestamp;
        
	/// Metric name
	const std::string name;

	/// Metric entity (origin)
        const std::string entity;

public:

	/// Initialize class variables (r-value constructor)
        Metric(std::string&& _name, std::string&& _entity, std::chrono::time_point<std::chrono::system_clock>&& _timestamp);

	/// Default destructor
	virtual ~Metric() = default;
        
	/// Name getter
	/// \return	metric name
	std::string getName();

	/// Timestamp getter
	/// \return 	metric timestamp
        std::chrono::time_point<std::chrono::system_clock> getTimestamp();

	/// Sends metric through backends
	/// \param b	poiner to backend
        virtual void sendViaBackend(Backend * b) = 0;

	/// Subtracts values of two metrics
	/// \param lhs 	metric that value is subtracted from
        virtual Metric* subtract(Metric* lhs) = 0;

	/// Calculate average value of metrics vector
	/// \param metrics 	reference to metrics vector
	/// \return 		metric with average value calculated
	virtual Metric* average(const std::vector<Metric*> &metrics) = 0;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
