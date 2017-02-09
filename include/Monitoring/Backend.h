///
/// \file Backend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_BACKEND_H
#define ALICEO2_MONITORING_CORE_BACKEND_H

#include <chrono>
#include <string>
#include "Monitoring/Metric.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core 
{

/// Backend pure virtual interface
///
/// Specifies overloaded send function that represents process of sending a metric to remote backend.
/// Each metric consists of four parameters: value, name, entity (origin), timestamp.
/// Supported types of "value" are: int, double, string and uint_32t
class Backend
{
  public:
    /// Default constructor
    Backend() = default;

    /// Default destructor
    virtual ~Backend() = default;
	
    virtual void send(const Metric& metric) = 0;
    virtual void addGlobalTag(std::string name, std::string value) = 0;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_BACKEND_H
