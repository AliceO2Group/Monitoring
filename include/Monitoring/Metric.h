///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <vector>
#include <boost/variant.hpp>
#include "Tag.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

enum MetricType { INT = 0, STRING = 1, DOUBLE = 2, UINT64_T = 3 };

/// \brief Represents a metric including value, type of the value, name, timestamp and tags
class Metric
{
  public:
    /// Integer metric construtor
    /// \param value 	 	metric value (int)
    /// \param name 	 	metric name
    /// \param timestamp 	metric timestamp in milliseconds
    Metric(int value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// String metric construtor
    /// \param value            metric value (string)
    /// \param name             the metric name
    /// \param timestamp        metric timestamp in milliseconds    
    Metric(std::string value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// Double metric constructor
    /// \param value            metric value (double)
    /// \param name             metric name
    /// \param timestamp        metric timestamp in milliseconds 
    Metric(double value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// uint64_t metric constructor
    /// \param value            metric value (uint64_t)
    /// \param name             metric name
    /// \param timestamp        metric timestamp in milliseconds
    Metric(uint64_t value, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// boost variant metric constructor, required by derived metrics logic
    /// \param value            metric value (boost variant)
    /// \param name             metric name
    /// \param timestamp        metric timestamp in milliseconds
    Metric(boost::variant< int, std::string, double, uint64_t >, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

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
    boost::variant< int, std::string, double, uint64_t > getValue() const;

    /// Value type getter
    /// \return type of value stores inside metric : 0 - int, 1 - std::string, 2 - double
    int getType() const;

    /// Tag list getter
    /// \return         vector of tags
    std::vector<Tag> getTags() const;

    /// Add user defined tags
    /// \param tags      r-value to vector of tags
    /// \return          r-value to "this" - to be able to chain methods
    Metric&& addTags(std::vector<Tag>&& tags);

    /// Generetes current timestamp
    /// return          timestamp as std::chrono::system_clock
    static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

  protected:
    /// Metric value
    boost::variant< int, std::string, double, uint64_t > mValue;

    /// Metric name
    std::string mName;

    /// Metric timestamp
    std::chrono::time_point<std::chrono::system_clock> mTimestamp;

    /// Metric tags
    std::vector<Tag> tagSet;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
