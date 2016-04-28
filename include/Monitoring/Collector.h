#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "Configuration/Configuration.h"
#include "Monitoring/MonInfoLogger.h"
#include "Monitoring/Backend.h"
#include "Monitoring/Metric.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
	/// Avaliable derived metric modes : RATE and AVERAGE values
	enum class DerivedMetricMode { RATE, AVERAGE };

class Collector {

private:
	/// Loaded configuration file.
	ConfigFile mConfigFile;

	/// Vector of backends (where the values are send to).
	std::vector <Backend*> backends;

	/// Cache of registered metrics (metric name / vector of metric pointers).
        std::map <std::string, std::vector<Metric*>> cache;

	/// Registered metrics with their modes (metric name, registered mode).
	/// See list of modes in begiing of the file.
	std::map <std::string, DerivedMetricMode> registered;
public:
	/// Reads configuration file and based on it loaded backends.
        Collector();

	/// Generates timestamp in miliseconds
	/// \return timestamp as unsigned long
	static std::chrono::time_point<std::chrono::system_clock> getCurrentTimestamp();
	
	/// Stores past metrics in container in order to allow create new metrics based on them
	/// (eg. metric rate, average value).
	/// Then, it sends the metric to all of backends (stored in "backends" vector)
	
	/// param value of the metric
	/// param name of the metric
	/// param entity where the metric come from
	/// param tiemstamp in miliseconds, if not provided output of getCurrentTimestamp as default value is assigned
	template<typename T> void send(T value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp = Collector::getCurrentTimestamp());
  
	/// Registers metric - to be processed in one of the modes
	/// Following processing modes are supported: REGISTER_RATE, REGISTER_AVERAGE
	void addDerivedMetric(DerivedMetricMode mode, std::string name);

	/// Handles metric processing, switches over processing modes
	void processMetric(DerivedMetricMode mode, Metric* metric);

	/// Inserts metric into std::map cache
	/// param metric pointer to metric object
	void insert(Metric* metric);

	/// Calculates rate based on past and curret value and timestamp      
	/// \param name
	void injectRate(std::string name);

	/// Calculates average value based on all past values
	/// param name
	void injectAverage(std::string name);

	/// Deallocates all the memory (metrics and backends)
	/// TODO : move to smart pointers
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
