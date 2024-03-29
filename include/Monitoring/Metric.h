// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file Metric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_METRIC_H
#define ALICEO2_MONITORING_CORE_METRIC_H

#include <string>
#include <chrono>
#include <map>
#include <regex>
#include <variant>
#include <vector>
#include "Tags.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// Metric and Backedn verbosity
enum class Verbosity : short { Prod,
                               Info,
                               Debug };

/// Metric types
enum MetricType { INT = 0,
                  STRING = 1,
                  DOUBLE = 2,
                  UINT64_T = 3 };

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

  /// Constructor that  does not require any value to be specified, .addValue needs to be used
  /// \param name             metric name
  Metric(const std::string& name, Verbosity verbosity = Metric::DefaultVerbosity, const std::chrono::time_point<std::chrono::system_clock>& timestamp = Metric::getCurrentTimestamp());

  /// Adds additional int value to metric
  /// \param value
  /// \param name
  Metric&& addValue(int value, const std::string& name);

  /// Adds additional double value to metric
  /// \param value
  /// \param name
  Metric&& addValue(double value, const std::string& name);

  /// Adds additional uint64_t value to metric
  /// \param value
  /// \param name
  Metric&& addValue(uint64_t value, const std::string& name);

  /// Adds additional string value to metric
  /// \param value
  /// \param name
  Metric&& addValue(std::string value, const std::string& name);

  /// Adds additional variant value to metric
  /// \param value
  /// \param name
  Metric&& addValue(const std::variant<int, std::string, double, uint64_t>& value, const std::string& name);

  /// Increases metric value by provided value
  /// \param value   value that metric will be increased
  /// \param name    value name
  Metric&& increaseValue(const std::variant<int, std::string, double, uint64_t>& value, const std::string& name = "value");

  /// Default destructor
  ~Metric() = default;

  /// Name getter
  /// \return	metric name
  const std::string& getName() const;

  /// Timestamp getter
  /// \return 	metric timestamp
  std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;

  /// Values getter
  /// \return vector of values
  const std::vector<std::pair<std::string, std::variant<int, std::string, double, uint64_t>>>& getValues() const;

  /// First value getter
  /// \return first value as pair
  const std::pair<std::string, std::variant<int, std::string, double, uint64_t>>& getFirstValue() const;

  /// Values vector size getter
  std::size_t getValuesSize() const noexcept;

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

  /// Whether timestamp should be included or not
  static bool includeTimestamp;

  /// This is required for backward compability with boost::variant and old StdOut format
  int getFirstValueType() const;

 protected:
  /// Allow DerivedMetrics access to setTags
  friend class o2::monitoring::DerivedMetrics;

  /// Set full vector of tags
  Metric&& setTags(std::vector<std::pair<int, int>>&& tags);

  /// Metric values
  std::vector<std::pair<std::string, std::variant<int, std::string, double, uint64_t>>> mValues;

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

  /// Default value name
  static constexpr char mDefaultValueName[] = "value";
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_METRIC_H
