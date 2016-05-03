#ifndef ALICEO2_MONITORING_CORE_TEMPLATED_METRIC_H
#define ALICEO2_MONITORING_CORE_TEMPLATED_METRIC_H

#include <string>
#include <vector>
#include "Monitoring/Metric.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {


/// \brief Templated class that stores different type of metric values
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
template <class T>
class TemplatedMetric : public Metric
{
        const T value; ///< metric value

public:
	/// Construct Metric object by moving parameters to Metric constructor
        TemplatedMetric(T value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);

	/// Sends metric through backends
	/// \param b    poiner to backend 
        void sendViaBackend(Backend * b);
	
	/// Value getter
	/// return 	value
	T getValue();

	/// Subtracts values of two metrics
	/// \param m 	metric that value is subtracted from
        Metric* subtract(Metric* m);

	/// Calculate average value of metrics vector
	/// \param metrics      reference to metrics vector
	/// \return             metric with average value calculated
	Metric* average(const std::vector<Metric*> &metrics);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_TEMPLATED_METRIC_H
