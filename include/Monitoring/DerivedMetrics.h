///
/// \file DerivedMetrics.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
#define ALICEO2_MONITORING_CORE_DERIVED_METRICS_H

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "Monitoring/Metric.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
	
/// Available derived metric modes : RATE and AVERAGE values
enum class DerivedMetricMode { RATE, INCREMENT, NONE };

/// Calculates derived metrics
///
/// Calculates derived metrics such as rate or average values... (available modes are enumerated in DerivedMetricMode class)
/// For this reason past metrics are stored in a container (std::map).
/// It applies only to metrics registered via #registerMetric method
class DerivedMetrics
{
  public:
    /// Default constructor
    DerivedMetrics() = default;

    /// Default destructor
    ~DerivedMetrics() = default;

    /// Calculates rate value based on metrics stored in mCache map
    /// \param name 	metric name
    /// \return 	metric with calculated rate value
    Metric rate(Metric& metric);
    Metric increment(Metric& metric);

    /// Cache of registered metrics (metric name / vector of metric pointers).
    std::unordered_map <std::string, Metric> mStorage;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
