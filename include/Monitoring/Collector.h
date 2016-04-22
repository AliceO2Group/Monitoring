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
	std::vector <Backend*> backends;
        std::map <std::string, std::vector<Metric*>> cache;
public:
        Collector();
	static unsigned long getCurrentTimestamp();
	template<typename T> void sendProcessed(T value, std::string name, std::string entity, unsigned long timestamp = Collector::getCurrentTimestamp());
        template<typename T> void send(T value, std::string name, std::string entity, unsigned long timestamp = Collector::getCurrentTimestamp());
        void injectRate(std::string name);
	void insert(Metric* metric);
        ~Collector();

};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_COLLECTOR_H
