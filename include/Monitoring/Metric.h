///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <boost/variant.hpp>
#include "../src/Tag.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

enum MetricType { INT = 0, STRING = 1, DOUBLE = 2, UINT32_T = 3 };

/// \brief Represents metric parameters except (value, name, entity and timestamp)
class Metric
{
  public:
    /// Initialize class variables
    /// \param value 	 	metric value (int)
    /// \param name 	 	metric name
    /// \param timestamp 	metric timestamp in milliseconds
    Metric(int value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());
    /// Initialize class variables
    /// \param value            metric value (string)
    /// \param name             the metric name
    /// \param timestamp        metric timestamp in milliseconds    
    Metric(std::string value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// Initialize class variables
    /// \param value            metric value (double)
    /// \param name             metric name
    /// \param timestamp        metric timestamp in milliseconds 
    Metric(double value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// Initialize class variables
    /// \param value            metric value (uint32_t)
    /// \param name             metric name
    /// \param timestamp        metric timestamp in milliseconds 
    Metric(uint32_t value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());
	
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

    /// Value type getter
    /// \return type of value stores inside metric : 0 - int, 1 - std::string, 2 - double, 3 - uint32_t
    int getType() const;

    /// Tag list getter
    /// \return         vector of tags
    std::vector<Tag> getTags() const;

    /// Sets user defined timestamp
    /// \param           timestamp
    /// \return          r-value to "this" - to be able to chain methods
    Metric&& setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp);
 
    /// Add user defined tags
    /// \param           r-value to vector of tags
    /// \return          r-value to "this" - to be able to chain methods
    Metric&& addTags(std::vector<Tag>&& tags);

    /// Generetes current timestamp
    /// return          timestamp as std::chrono::system_clock
    static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

  private:
    /// Metric value
    boost::variant< int, std::string, double, uint32_t > mValue;

    /// Metric name
    std::string mName;

    /// Metric timestamp
    std::chrono::time_point<std::chrono::system_clock> mTimestamp;

    /// Metric tags
    std::vector<Tag> tagSet;
};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
