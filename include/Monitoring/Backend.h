#ifndef ALICEO2_MONITORING_CORE_BACKEND_H
#define ALICEO2_MONITORING_CORE_BACKEND_H

#include <chrono>
#include <string>

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {


class Backend {
public:
	virtual void send(int value, 	std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) = 0;
	virtual void send(double value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) = 0;
	virtual void send(std::string value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) = 0;
	virtual void send(uint32_t value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) = 0;
	virtual ~Backend() = default;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_BACKEND_H
