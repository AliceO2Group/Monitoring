///
/// \file ComplexMetric.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_COMPLEXMETRIC_H
#define ALICEO2_MONITORING_CORE_COMPLEXMETRIC_H

#include "Metric.h"
#include <mutex>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

// \brief Extends metric to value setter
class ComplexMetric : public o2::monitoring::Metric
{
  public:
    /// Integer metric construtor
    /// \param value 	 	metric value (int)
    /// \param name 	 	metric name
    ComplexMetric(int value, const std::string& name);

    /// String metric construtor
    /// \param value            metric value (string)
    /// \param name             the metric name
    ComplexMetric(std::string value, const std::string& name);

    /// Double metric constructor
    /// \param value            metric value (double)
    /// \param name             metric name
    ComplexMetric(double value, const std::string& name);

    /// uint64_t metric constructor
    /// \param value            metric value (uint64_t)
    /// \param name             metric name
    ComplexMetric(uint64_t value, const std::string& name);

    /// boost variant metric constructor, required by derived metrics logic
    /// \param value            metric value (boost variant)
    /// \param name             metric name
    ComplexMetric(boost::variant< int, std::string, double, uint64_t >, const std::string& name);

    /// Default destructor
    ~ComplexMetric() = default;

    // Resets metric's timestamp
    void resetTimestamp();

    /// Assign operator overload, assignes new values to the metric object
    ComplexMetric& operator=(const boost::variant< int, std::string, double, uint64_t >& value);
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_COMPLEXMETRIC_H
