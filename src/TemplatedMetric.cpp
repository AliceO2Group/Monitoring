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
void TemplatedMetric<T>::print()
{
	std::cout << value << name << entity << timestamp << std::endl;
}
template <class T>
TemplatedMetric<T>::TemplatedMetric(T value, std::string name, std::string entity, unsigned long timestamp):
        Metric( std::move(name), std::move(entity), timestamp ),
        value ( value )
        {}

template<class T>
T TemplatedMetric<T>::getValue()
{
	return value;
}
/*template <class T>
double TemplatedMetric<T>::add(Metric *m)
{
	
}
*/
template <class T>
Metric* TemplatedMetric<T>::substract(Metric *m)
{
	double substract = 1000*((value - static_cast<TemplatedMetric<T>*>(m)->getValue()) / (timestamp - m->getTimestamp()));
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
