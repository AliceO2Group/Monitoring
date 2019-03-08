///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <vector>
#include <regex>
#include <boost/variant.hpp>
#include "Tags.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// Metric and Backedn verbosity
enum class Verbosity : short { Prod, Info, Debug };


/// Metric types
enum MetricType { INT = 0, STRING = 1, DOUBLE = 2, UINT64_T = 3 };

class DerivedMetrics;

/// \brief Represents a metric including value, type of the value, name, timestamp and tags
class Metric
{
  public:
    /// Integer metric construtor
    /// \param value 	 	metric value (int)
    /// \param name 	 	metric name
    Metric(int value, const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity);

    /// String metric construtor
    /// \param value            metric value (string)
    /// \param name             the metric name
    Metric(std::string value, const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity);

    /// Double metric constructor
    /// \param value            metric value (double)
    /// \param name             metric name
    Metric(double value, const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity);

    /// uint64_t metric constructor
    /// \param value            metric value (uint64_t)
    /// \param name             metric name
    Metric(uint64_t value, const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity);

    /// boost variant metric constructor, required by derived metrics logic
    /// \param value            metric value (boost variant)
    /// \param name             metric name
    Metric(std::variant< int, std::string, double, uint64_t >, const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity);

    /// Default destructor
    ~Metric() = default;

    /// Assign operator overload, assignes new values to the metric object
    Metric& operator=(const std::variant< int, std::string, double, uint64_t >& value);

    /// Name getter
    /// \return	metric name
    const std::string& getName() const;

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
    /// \return         tags
    const std::vector<std::pair<int, int>>& getTags() const;

    /// Add user defined tags
    /// \param key      enum tag key
    /// \param value 	emum tag value
    /// \return         r-value to "this" - to be able to chain methods
    Metric&& addTag(tags::Key key, tags::Value value);

    /// Add user defined tags
    /// \param key      enum tag key
    /// \param value    numeric value
    /// \return          r-value to "this" - to be able to chain methods
    Metric&& addTag(tags::Key key, unsigned short int number);

    /// Verbosity getter
    Verbosity getVerbosity();

    /// Generetes current timestamp
    /// return          timestamp as std::chrono::system_clock
    static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

    /// Sets default verbosity of metrics
    static void setDefaultVerbosity(Verbosity verbosity);

    /// Default metric verbosity
    static Verbosity DefaultVerbosity;

    /// Regex policy setter
    static void setVerbosityPolicy(Verbosity verbosity, const std::regex& regex);
  protected:
    /// Allow DerivedMetrics access to setTags
    friend class o2::monitoring::DerivedMetrics;

    /// Set full vector of tags
    Metric&& setTags(std::vector<std::pair<int, int>>&& tags);

    /// Metric value
    std::variant< int, std::string, double, uint64_t > mValue;

    /// Metric name
    std::string mName;

    /// Metric timestamp
    std::chrono::time_point<std::chrono::system_clock> mTimestamp;

    /// Metric tags
    std::vector<std::pair<int, int>> mTags;

    /// Metric verbosity
    Verbosity mVerbosity;

    /// Regex policy map
    static std::map<std::underlying_type<Verbosity>::type, std::regex> mRegexPolicy;

    /// Overwirte verbosity using regex policy
    void overwriteVerbosity();
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
