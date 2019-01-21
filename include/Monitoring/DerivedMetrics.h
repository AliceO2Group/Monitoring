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
	
/// Available derived metric modes
enum class DerivedMetricMode { RATE, INCREMENT, NONE };

/// \brief Enables Calculation of derived metrics
///
/// Calculates derived metrics such as rate or increment values... (available modes are enumerated in DerivedMetricMode class)
/// For this purpose metrics are stored in a container.
class DerivedMetrics
{
  public:
    /// Default constructor
    DerivedMetrics() = default;

    /// Default destructor
    ~DerivedMetrics() = default;

    /// Metrics store necessary for derived metrics
    std::unordered_map <std::string, Metric> mStorage;

    /// Entry method to DerivedMetrics
    /// Switches over processing modes: rate and increment
    Metric process(Metric& metric, DerivedMetricMode mode);

    std::string getTagsHash(const std::vector<std::variant<tags::Detector, tags::Subsystem>>& tags);
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
