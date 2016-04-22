#include <iostream>
#include "Monitoring/MonInfoLogger.h"
#include "Monitoring/StdoutBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

void StdoutBackend::send(const int value, const std::string name, const std::string entity, const unsigned long timestamp)
{
	MonInfoLogger::GetInstance() << "Monitoring : Int( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const double value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : Double( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const std::string value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : String( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const uint32_t value, const std::string name, const std::string entity, const unsigned long timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : Uint32_t( " << name << ", " << value << ", " << timestamp << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

