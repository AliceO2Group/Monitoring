#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "Configuration/Configuration.h"
#include "Monitoring/Backend.h"
#include "Monitoring/Metric.h"
#include "Monitoring/DerivedMetrics.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {


/// \brief Collects metrics - public interface
///
/// Collects metrics and pushes them through all avaliable backends. Supported types: int, double, string, uint32_t.
/// Supports feature of calculating derived metrics, such as rate and average value (see #addDerivedMetric method).
/// Generates default entity (origin) as concatanated hostname and process ID.
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class Collector {

private:
	/// Loaded configuration file.
	ConfigFile mConfigFile;

	/// Object responsible from derived metrics
	/// \see class DerivedMetrics
	DerivedMetrics *derivedHandler;

	/// Vector of backends (where the values are send to).
	std::vector <Backend*> backends;

	/// Default entity value, see setUniqueEntity method
	std::string uniqueEntity;

	/// Generates entity value as concatenated hostname and process id
	void setUniqueEntity();

	/// Sends metric object through all backends
	void sendMetric(Metric* metric);

public:
	/// Initialaze backends and instance of "derived metric processor" (DerivedMetrics class)
	/// \param configurationPath 	filepath to AppMon configuration file
        Collector(std::string configurationPath);

	/// Generates timestamp in miliseconds
	/// \return timestamp as unsigned long
	static std::chrono::time_point<std::chrono::system_clock> getCurrentTimestamp();

	/// Overwrites default entity value
	/// \param entity 	new entity value
	void setEntity(std::string entity);

	/// Sends the metric to all avaliabes backends
	/// If metric has been added to "derived metric processor" the derived metric is calculated (see addDerivedMetric method)
	/// \param value of the metric
	/// \param name of the metric
	/// \param timestamp in miliseconds, if not provided output of getCurrentTimestamp as default value is assigned
	template<typename T> void send(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp());
  
	/// Adds derived metric - each time the metric arrives the derived metric is calculated and set to all backends
	/// Following processing modes are supported: DerivedMetricMode::RATE, DerivedMetricMode::AVERAGE
	/// \param mode
	/// \param name
	void addDerivedMetric(DerivedMetricMode mode, std::string name);
	
	/// Deallocates all the memory (metrics and backends)
	/// \todo : move to smart pointers
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
