#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <vector>
#include "Monitoring/Backend.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

class Metric {
protected:
        const unsigned long timestamp;
        const std::string name;
        const std::string entity;

public:
        Metric(std::string&& _name, std::string&& _entity, unsigned long _timestamp);
        std::string getName();
        unsigned long getTimestamp();
        virtual void print() = 0;
        virtual void sendViaBackend(Backend * b) = 0;
        virtual ~Metric() = default;
        virtual Metric* substract(Metric* lhs) = 0;
	virtual Metric* average(const std::vector<Metric*> &metrics) = 0;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
