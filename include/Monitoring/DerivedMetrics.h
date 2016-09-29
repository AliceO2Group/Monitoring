///
/// \file DerivedMetrics.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
#define ALICEO2_MONITORING_CORE_DERIVED_METRICS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Monitoring/Metric.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{
	
/// Available derived metric modes : RATE and AVERAGE values
enum class DerivedMetricMode { RATE, AVERAGE };

/// Calculates derived metrics
///
/// Calculates derived metrics such as rate or average values... (available modes are enumerated in DerivedMetricMode class)
/// For this reason past metrics are stored in a container (std::map).
/// It applies only to metrics registered via #registerMetric method
class DerivedMetrics
{
  public:
    /// Default constructor
    DerivedMetrics(const unsigned int cacheSize);

    /// Default destructor
    ~DerivedMetrics() = default;

    /// States whether metric has been registered or not
    /// \param name metric name
    /// \return 	true when metric is present in mRegistered, false otherwise
    bool isRegistered(std::string name);

    /// Registers metric to be processed (adds its name to mRegistered map)
    /// Processing modes are enumerated in DerivedMetricMode class
    /// \param name 	name, metrics name
    /// \param mode     mode, see DerivedMetricMode
    void registerMetric(std::string name, DerivedMetricMode mode);

    /// Handles actual metric processing; finds out whether metric needs to be processed or not
    /// If yes, passing it to one of methods that handles calculation of derived metric
    /// \param value 		metric value
    /// \param name 		metric name
    /// \param entity 		metric entity
    /// \param timestamp 	metric timestamp
    /// \return Metric object with calculated derived metric in it
    template<typename T> 
    std::unique_ptr<Metric> processMetric(T value, std::string name, std::string entity, 
                                        std::chrono::time_point<std::chrono::system_clock> timestamp);
  
  private:
    /// Calculates rate value based on metrics stored in mCache map
    /// \param name 	metric name
    /// \return 	metric with calculated rate value
    template<typename T> std::unique_ptr<Metric> calculateRate(std::string name, T);

    /// Calculates average value based on metrics stored in mCache map
    /// \param name 	metric name
    /// \return		metric with calculated average value
    template<typename T> std::unique_ptr<Metric> calculateAverage(std::string name, T);

    /// maximum size of cache map
    const unsigned int mMaxVectorSize;

    /// Cache of registered metrics (metric name / vector of metric pointers).
    std::map <std::string, std::vector<std::unique_ptr<Metric>>> mCache;

    /// Registered metrics with their modes (metric name, registered mode).
    std::map <std::string, DerivedMetricMode> mRegistered;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
