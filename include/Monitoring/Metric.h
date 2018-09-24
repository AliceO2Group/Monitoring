///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <mutex>
#include <vector>
#include <variant>
#include "Tag.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

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
    Metric(std::variant< int, std::string, double, uint64_t >, const std::string& name, std::chrono::time_point<std::chrono::system_clock> timestamp = Metric::getCurrentTimestamp());

    /// Default destructor
    ~Metric() = default;

    /// Copy initialization
    Metric(const Metric& other);

    /// Copy assignment
    Metric& operator=(Metric const& other);

    /// Assign operator overload, assignes new values to the metric object
    Metric& operator=(const std::variant< int, std::string, double, uint64_t >& value);
        
    /// Name getter
    /// \return	metric name
    std::string getName() const;

    /// Name setter
    /// \param name	new name of the metric
    void setName(std::string name);

    /// Timestamp getter
    /// \return 	metric timestamp
    std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;
	
    /// Value getter
    /// \return metric value
    std::variant< int, std::string, double, uint64_t > getValue() const;

    /// Value type getter
    /// \return type of value stores inside metric : 0 - int, 1 - std::string, 2 - double
    int getType() const;

    /// Tag list getter
    /// \return         vector of tags
    std::vector<Tag> getTags() const;

    /// Sets user defined timestamp
    /// \param timestamp	timestamp to set
    /// \return         	 r-value to "this" - to be able to chain methods
    Metric&& setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp);
 
    /// Add user defined tags
    /// \param tags      r-value to vector of tags
    /// \return          r-value to "this" - to be able to chain methods
    Metric&& addTags(std::vector<Tag>&& tags);

    /// Generetes current timestamp
    /// return          timestamp as std::chrono::system_clock
    static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

  private:
    /// Metric value
    std::variant< int, std::string, double, uint64_t > mValue;

    /// Metric name
    std::string mName;

    /// Metric timestamp
    std::chrono::time_point<std::chrono::system_clock> mTimestamp;

    /// Metric tags
    std::vector<Tag> tagSet;

    /// Mutex for accesing metric value
    mutable std::mutex mValueMutex;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
