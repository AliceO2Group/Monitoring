#include <iostream>
#include "Monitoring/MonInfoLogger.h"
#include "Monitoring/StdoutBackend.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {


inline unsigned long StdoutBackend::convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	return std::chrono::duration_cast <std::chrono::milliseconds>(
		timestamp.time_since_epoch()
        ).count();
}

void StdoutBackend::send(const int value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	MonInfoLogger::GetInstance() << "Monitoring : Int( " << name << ", " << value << ", " << convertTimestamp(timestamp) << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const double value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : Double( " << name << ", " << value << ", " << convertTimestamp(timestamp) << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const std::string value, const std::string name, const std::string entity, const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : String( " << name << ", " << value << ", " << convertTimestamp(timestamp) << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;

}
void StdoutBackend::send(const uint32_t value, const std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
        MonInfoLogger::GetInstance() << "Monitoring : Uint32_t( " << name << ", " << value << ", " << convertTimestamp(timestamp) << ", " << entity << ")" << AliceO2::InfoLogger::InfoLogger::endm;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

