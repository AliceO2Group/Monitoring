/// \file    DataCollector.h
/// \brief   Definition of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#ifndef ALICEO2_MONITORING_CORE_DATA_COLLECTOR_APMON_H
#define ALICEO2_MONITORING_CORE_DATA_COLLECTOR_APMON_H

#include "DataCollector.h"
#include <ApMon.h>
#include <utils.h>
#include <string>

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
/// Interface to Monitoring system
///
/// Interface to Monitoring system, allowing any process
/// to send application-specific values
class DataCollectorApMon : public DataCollector
{
  public:
    /// Standard constructor
    /// \param  configurationFile  Path to configuration file
    DataCollectorApMon(const std::string configurationFile);

    /// Destructor
    virtual ~DataCollectorApMon();

    /// Get full path to configuration file
    /// \return  Return full path to configuration file
    const std::string& getConfigurationFile() const;

    /// Send integer value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    void sendValue(std::string cluster, std::string node, std::string metric, int value);

    /// Send double value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    void sendValue(std::string cluster, std::string node, std::string metric, double value);

    /// Send string value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    void sendValue(std::string cluster, std::string node, std::string metric, std::string value);

    /// Send timed integer value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    void sendTimedValue(std::string cluster, std::string node, std::string metric, int value, int timestamp);

    /// Send timed double value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    void sendTimedValue(std::string cluster, std::string node, std::string metric, double value, int timestamp);

    /// Send timed string value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    void sendTimedValue(std::string cluster, std::string node, std::string metric, std::string value, int timestamp);

    /// Log message
    /// \param  logLevel    Log level
    /// \param  message     Log message
    void log(int logLevel, std::string message);

  private:
    /// Get ApMon
    /// \return  Return ApMon object
    ApMon* getApMon() const;

    /// Configure automatic monitoring of process metrics (CPU, mem, etc.)
    void configureProcessMonitoring();

    /// Get default cluster name
    std::string getDefaultCluster() const;

    const std::string mConfigurationFile; ///< Path to configuration file
    ApMon* mApMon;                        ///< ApMon object
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DATA_COLLECTOR_APMON_H
