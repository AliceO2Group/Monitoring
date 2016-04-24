#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include "Configuration/Configuration.h"
#include "Monitoring/MonInfoLogger.h"
#include "Monitoring/Backend.h"
#include "Monitoring/Metric.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
	/// Avaliable modes
	/// Rate
	const int REGISTER_RATE = 1;
	/// Average value
	const int REGISTER_AVERAGE = 2;

class Collector {

private:
        ConfigFile mConfigFile;

	/// Vector of backends, values are sent to all of them
	std::vector <Backend*> backends;

	/// Cache of registered metrics (metric name / vector of metric pointers)
        std::map <std::string, std::vector<Metric*>> cache;

	/// Registered metrics with their modes (metric name, registered mode)
	std::map <std::string, int> registered;
public:
        Collector();

	/// Generates timestamp in microseconds
	/// \return timestamp as unsigned long
	static unsigned long getCurrentTimestamp();

	/// Keeps past metric in container and allows to process them to calculate needed values (eg. metric rate, average value)
	/// Then, it sends the metric to backends
	/// param value value of the metric
	/// param name name of the metric
	/// param entity where the metric come from
	/// param tiemstamp timestamp in miliseconds; by default output of getCurrentTimestamp is assigned
	template<typename T> void send(T value, std::string name, std::string entity, unsigned long timestamp = Collector::getCurrentTimestamp());
  
	/// Calculates rate bansed on values and timestamps of current and previous metric
	/// not aplicable for strings
	void injectRate(std::string name);

	/// Register metric to be processed
	/// Following processing modes are supported: REGISTER_RATE, REGISTER_AVERAGE
	void registerMetric(std::string name, int mode);

	/// Handles metric processing, switches over processing modes
	void processMetric(int mode, Metric* metric);

	/// Inserts metric into std::map cache
	/// param metric pointer to metric object
	void insert(Metric* metric);
	void injectAverage(std::string name);
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
