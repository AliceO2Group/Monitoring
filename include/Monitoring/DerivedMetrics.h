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
	
/// Avaliable derived metric modes : RATE and AVERAGE values
enum class DerivedMetricMode { RATE, AVERAGE };

/// \brief Calculates derived metrics (rate, average...)
///
/// Allows to calcualte derived metrics such as rate and average values... (avaliable modes in DerivedMetricMode class)
/// For this reason stores past values in std::map.
/// It applies only to metrics registered via #registerMetric method (see where Collector invokes this method)
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class DerivedMetrics
{
  public:
    /// Default constructor
    DerivedMetrics(const unsigned int cacheSize);

    /// Default destructor
    ~DerivedMetrics() = default;

    /// States whether metric has been registered or not
    /// \param name metric name
    /// \return 	yes or no
    bool isRegistered(std::string name);

    /// Registers metric - to be processed in one of the modes
    /// Following processing modes are supported: REGISTER_RATE, REGISTER_AVERAGE
    /// \param mode 	mode, see DerivedMetricMode
    /// \param name 	name, metrics name
    void registerMetric(DerivedMetricMode mode, std::string name);

    /// Handles metric processing, switches over processing modes
    template<typename T> 
    std::unique_ptr<Metric> processMetric(T value, std::string name, std::string entity, 
                                        std::chrono::time_point<std::chrono::system_clock> timestamp);
  
  private:
    /// Calculates rate based on past and curret value and timestamp
    /// \param name 	metric name
    /// \return 		metric with calculated rate value
    template<typename T> std::unique_ptr<Metric> calculateRate(std::string name, T);

    /// Calculates average value based on all past values
    /// \param name 	metric name
    /// \return		metric with calculated average value
    template<typename T> std::unique_ptr<Metric> calculateAverage(std::string name, T);

    /// maximum size of vector in cache
    const unsigned int mMaxVectorSize;

    /// Cache of registered metrics (metric name / vector of metric pointers).
    std::map <std::string, std::vector<std::unique_ptr<Metric>>> mCache;

    /// Registered metrics with their modes (metric name, registered mode).
    /// See list of modes in begiing of the file.
    std::map <std::string, DerivedMetricMode> mRegistered;
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DERIVED_METRICS_H
