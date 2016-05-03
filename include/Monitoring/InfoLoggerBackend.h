#ifndef ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H
#define ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H

#include <string>
#include "Monitoring/Backend.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {

/// \brief Backend that injects metrics to InfoLogger
///
/// Inject monitoring metrics to InfoLogger - logging module
/// InfoLogger does not support std::chrono::time_point therefore timestamps is converted to unsigned long (see method #convertTimestamp)
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class InfoLoggerBackend : public Backend {
public:
	/// Default constructor
	InfoLoggerBackend() = default;

	/// Default destructor
	~InfoLoggerBackend() = default;
	
	/// Pushes integer metric
	/// \param value        metric value (integer)
	/// \param name         metric name
	/// \param entity       metric entity - origin
	/// \param timestamp    metric timestamp (std::chrono::time_point)
        void send(int value, std::string name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;

	/// Pushes double metric
	/// \param value        metric value (double)
	/// \param name         metric name
	/// \param entity       metric entity - origin
	/// \param timestamp    metric timestamp (std::chrono::time_point
        void send(double value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;

	/// Pushes string metric
	/// \param value        metric value (string)
	/// \param name         metric name
	/// \param entity       metric entity - origin
	/// \param timestamp    metric timestamp (std::chrono::time_point)
        void send(std::string value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;

	/// Pushes uint32_t metric
	/// \param value        metric value (uint32_t)
	/// \param name         metric name
	/// \param entity       metric entity - origin
	/// \param timestamp    metric timestamp (std::chrono::time_point)
        void send(uint32_t value, std::string name, const std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp) override;

private:
	/// Converts timestamp to unsigned long (miliseconds from epoch)
	/// \param timestamp    timestamp in std::chrono::time_point format
	/// \return             timestamp as unsigned long (miliseconds from epoch)
	unsigned long convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_STDOUT_BACKEND_H
