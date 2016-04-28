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

class Metric {
protected:
        const std::chrono::time_point<std::chrono::system_clock> timestamp;
        const std::string name;
        const std::string entity;

public:
        Metric(std::string&& _name, std::string&& _entity, std::chrono::time_point<std::chrono::system_clock> _timestamp);
        std::string getName();
        std::chrono::time_point<std::chrono::system_clock> getTimestamp();
        virtual void sendViaBackend(Backend * b) = 0;
        virtual ~Metric() = default;
        virtual Metric* substract(Metric* lhs) = 0;
	virtual Metric* average(const std::vector<Metric*> &metrics) = 0;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
