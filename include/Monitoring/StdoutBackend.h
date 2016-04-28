#ifndef ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H
#define ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H

#include <string>
#include "Monitoring/Backend.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

class StdoutBackend : public Backend {
public:
        void send(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;
        void send(double value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;
        void send(std::string value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;
        void send(uint32_t value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;
	unsigned long convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H
