#ifndef ALICEO2_MONITORING_CORE_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include "Configuration/Configuration.h"
#include "Monitoring/Backend.h"
#include "Monitoring/Metric.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

class Collector {

private:
        ConfigFile mConfigFile;

	/// Vector of backends, values are sent to all of them
	std::vector <Backend*> backends;

	/// Storage of metrics to be processed
        std::map <std::string, std::vector<Metric*>> cache;
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
	template<typename T> void sendProcessed(T value, std::string name, std::string entity, unsigned long timestamp = Collector::getCurrentTimestamp());
        
	/// Passes metric to all avaliable backends
	/// 
	template<typename T> void send(T value, std::string name, std::string entity, unsigned long timestamp = Collector::getCurrentTimestamp());
        void injectRate(std::string name);

	/// Inserts metric into std::map cache
	/// param metric pointer to metric object
	void insert(Metric* metric);
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
