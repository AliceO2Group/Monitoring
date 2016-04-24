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

template <class T>
class TemplatedMetric : public Metric
{
        const T value;

public:
        TemplatedMetric(T value, std::string name, std::string entity, unsigned long timestamp);
        void print();
        void sendViaBackend(Backend * b);
	
	T getValue();
        Metric* substract(Metric* m);
	Metric* average(const std::vector<Metric*> &metrics);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_TEMPLATED_METRIC_H
