#include <iostream>
#include "Monitoring/TemplatedMetric.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

template <class T>
void TemplatedMetric<T>::sendViaBackend(Backend * b)
{
	b->send(value, name, entity, timestamp);	
}

template <class T>
TemplatedMetric<T>::TemplatedMetric(T value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp):
        Metric( std::move(name), std::move(entity), timestamp ),
        value ( value )
        {}

template<class T>
T TemplatedMetric<T>::getValue()
{
	return value;
}
template <class T>
Metric* TemplatedMetric<T>::average(const std::vector<Metric*> &metrics)
{
        T average = 0;
        for (auto const metric : metrics)
        {
                average += static_cast<TemplatedMetric<T>*>(metric)->getValue();
        }
	return new TemplatedMetric<T>(average, name + "Average", entity, timestamp);
}

template <class T>
Metric* TemplatedMetric<T>::substract(Metric *m)
{
	std::chrono::duration<double> timestampDifferenct = timestamp - m->getTimestamp();
	double substract = 1000*((value - static_cast<TemplatedMetric<T>*>(m)->getValue()) / timestampDifferenct.count());
	return new TemplatedMetric<double>(substract, name + "Rate", entity, timestamp);
}
template <>
Metric* TemplatedMetric<std::string>::substract(Metric *m)
{
        return nullptr;
}

template class TemplatedMetric<int>;
template class TemplatedMetric<double>;
template class TemplatedMetric<std::string>;
template class TemplatedMetric<uint32_t>;


} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
