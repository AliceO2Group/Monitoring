///
/// \file Backend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_BACKEND_H
#define ALICEO2_MONITORING_CORE_BACKEND_H

#include <chrono>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core 
{

/// \brief Interface for backends - pure virtual class
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class Backend
{
  public:
    /// Default constructor
    Backend() = default;

    /// Default destructor
    virtual ~Backend() = default;
	
    /// Sends integer metric
    /// \param value        metric value (integer)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    virtual void send(int value, const std::string& name, const std::string& entity, 
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) = 0;

    /// Sends double metric
    /// \param value        metric value (double)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    virtual void send(double value, const std::string& name, const std::string& entity, 
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) = 0;

    /// Sends string metric
    /// \param value        metric value (string)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    virtual void send(std::string value, const std::string& name, const std::string& entity, 
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) = 0;

    /// Sends uint32_t metric
    /// \param value        metric value (uint32_t)
    /// \param name         metric name
    /// \param entity       metric entity - origin
    /// \param timestamp    metric timestamp (std::chrono::time_point)
    virtual void send(uint32_t value, const std::string& name, const std::string& entity,
      const std::chrono::time_point<std::chrono::system_clock>& timestamp) = 0;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_BACKEND_H
