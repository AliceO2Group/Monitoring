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

class Collector {

private:
	/// Loaded configuration file.
	ConfigFile mConfigFile;

	/// Object responsible from derived metrics
	/// \see class DerivedMetrics
	DerivedMetrics *derivedHandler;

	/// Vector of backends (where the values are send to).
	std::vector <Backend*> backends;

	/// Cache of registered metrics (metric name / vector of metric pointers).
        std::map <std::string, std::vector<Metric*>> cache;

	/// Registered metrics with their modes (metric name, registered mode).
	/// See list of modes in begiing of the file.
	std::map <std::string, DerivedMetricMode> registered;

	/// Default entity value, see setUniqueEntity method
	std::string uniqueEntity;

	/// Generates entity value as concatenated hostname and process id
	void setUniqueEntity();

	/// Sends metric object through all backends
	void sendMetric(Metric* metric);

public:
	/// Reads configuration file and based on it loaded backends.
        Collector();

	/// Generates timestamp in miliseconds
	/// \return timestamp as unsigned long
	static std::chrono::time_point<std::chrono::system_clock> getCurrentTimestamp();

	/// Overwrites default entity value
	/// \param entity
	void setEntity(std::string entity);

	/// Stores past metrics in container in order to allow create new metrics based on them
	/// (eg. metric rate, average value).
	/// Then, it sends the metric to all of backends (stored in "backends" vector)
	/// \param value of the metric
	/// \param name of the metric
	/// \param tiemstamp in miliseconds, if not provided output of getCurrentTimestamp as default value is assigned
	template<typename T> void send(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp());
  
	/// Registers metric - to be processed in one of the modes
	/// Following processing modes are supported: REGISTER_RATE, REGISTER_AVERAGE
	void addDerivedMetric(DerivedMetricMode mode, std::string name);

	/// Deallocates all the memory (metrics and backends)
	/// \todo : move to smart pointers
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
