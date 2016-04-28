#ifndef ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
#define ALICEO2_MONITORING_CORE_DERIVED_METRICS_H

#include <string>
#include <vector>
#include <map>
#include "Monitoring/Metric.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
	/// Avaliable derived metric modes : RATE and AVERAGE values
	enum class DerivedMetricMode { RATE, AVERAGE };

class DerivedMetrics {

private:
	
	/// Cache of registered metrics (metric name / vector of metric pointers).
        std::map <std::string, std::vector<Metric*>> cache;

	/// Registered metrics with their modes (metric name, registered mode).
	/// See list of modes in begiing of the file.
	std::map <std::string, DerivedMetricMode> registered;

public:
        DerivedMetrics()  = default;
	~DerivedMetrics();
	/// States whether metric has been registered or not
	/// \return yes or no
	bool isRegistered(std::string name);

	/// Registers metric - to be processed in one of the modes
	/// Following processing modes are supported: REGISTER_RATE, REGISTER_AVERAGE
	void registerMetric(DerivedMetricMode mode, std::string name);

	/// Handles metric processing, switches over processing modes
	/// \return derived metric
	Metric* processMetric(Metric* metric);

	/// Inserts metric into std::map cache
	/// param metric pointer to metric object
	void insert(Metric* metric);

	/// Calculates rate based on past and curret value and timestamp      
	/// \param name
	/// \return derived metric
	Metric* calculateRate(std::string name);

	/// Calculates average value based on all past values
	/// \param name
	/// \return derived metric
	Metric* calculateAverage(std::string name);

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
