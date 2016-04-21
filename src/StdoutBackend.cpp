#include <iostream>
#include "Monitoring/StdoutBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

void StdoutBackend::send(const int value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	std::cout << "Monitoring : Int( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << std::endl;

}
void StdoutBackend::send(const double value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        std::cout << "Monitoring : Double( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << std::endl;

}
void StdoutBackend::send(const std::string value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        std::cout << "Monitoring : String( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << std::endl;

}
void StdoutBackend::send(const uint32_t value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        std::cout << "Monitoring : Uint32_t( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << std::endl;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

