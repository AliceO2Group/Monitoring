///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <boost/variant.hpp>
#include "Monitoring/Backend.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// \brief Represents metric parameters except (value, name, entity and timestamp)
class Metric
{
  public:
    /// Initialize class variables
    /// \param value 	 	metric value (int)
    /// \param name 	 	the metric nam
    /// \param timestamp 	metric timestamp in milliseconds
    Metric(int value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);

    /// Initialize class variables
    /// \param value            metric value (string)
    /// \param name             the metric nam
    /// \param timestamp        metric timestamp in milliseconds    
    Metric(std::string value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);

    /// Initialize class variables
    /// \param value            metric value (double)
    /// \param name             the metric nam
    /// \param timestamp        metric timestamp in milliseconds 
    Metric(double value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);

    /// Initialize class variables
    /// \param value            metric value (uint32_t)
    /// \param name             the metric nam
    /// \param timestamp        metric timestamp in milliseconds 
    Metric(uint32_t value, const std::string& name, std::string entity, std::chrono::time_point<std::chrono::system_clock> timestamp);
	
    /// Default destructor
    ~Metric() = default;
        
    /// Name getter
    /// \return	metric name
    std::string getName() const;

    /// Timestamp getter
    /// \return 	metric timestamp
    std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;
	
    /// Value getter
    /// \return metric value
    boost::variant< int, std::string, double, uint32_t > getValue() const;

    /// Entity getter
    /// \return metric entity
    std::string getEntity() const;

    /// Value type getter
    /// \return type of value stores inside metric : 0 - int, 1 - std::string, 2 - double, 3 - uint32_t
    int getType() const;

  protected:
    /// Metric value
    const boost::variant< int, std::string, double, uint32_t > mValue;

    /// Metric name
    const std::string mName;

    /// Metric entity (origin)
    const std::string mEntity;

    /// Metric timestamp
    const std::chrono::time_point<std::chrono::system_clock> mTimestamp;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
