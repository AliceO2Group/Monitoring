/// \file    DataCollector.h
/// \brief   Definition of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#ifndef ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H

#include <string>
#include <ApMon.h>
#include <utils.h>

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
/// Interface to Monitoring system
///
/// Interface to Monitoring system, allowing any process
/// to send application-specific values
class DataCollector
{
  public:
    /// Standard constructor
    /// \param  configurationFile  Path to configuration file
    /// \param  defaultCluster     Default cluster (e.g. FLP, EPN)
    //DataCollector(const std::string configurationFile, const std::string defaultCluster);

    /// Standard constructor
    /// \param  configurationFile  Path to configuration file
    /// \param  defaultCluster     Default cluster (e.g. FLP, EPN)
    /// \param  defaultNode        Default node (e.g. FLP-TPC-01)
    DataCollector(const std::string configurationFile, const std::string defaultCluster, const std::string defaultNode = "");

    /// Destructor
    virtual ~DataCollector();

    /// Get full path to configuration file
    /// \return  Return full path to configuration file
    const std::string& getConfigurationFile() const;

    /// Get default Cluster name
    /// \return  Return Cluster name
    std::string getDefaultCluster() const;

    /// Get default Node name
    /// \return  Return Node name
    std::string getDefaultNode();

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

    /// get hostname
    std::string& getHostname();

    /// Set hostname
    void setHostname();

    /// Get process unique identifier
    std::string& getProcessUniqueId();

    /// Set process unique identifier
    ///
    /// Concatenation of hostname and process id
    void setProcessUniqueId();

    /// Configure automatic monitoring of process metrics (CPU, mem, etc.)
    void configureProcessMonitoring();

    const std::string mConfigurationFile; ///< Path to configuration file
    std::string mDefaultCluster;          ///< Default Cluster name
    std::string mDefaultNode;             ///< Default Node name
    std::string mHostname;                ///< Hostname where process is running
    std::string mProcessName;             ///< Process name
    std::string mProcessUniqueId;         ///< Identifier for process metrics (hostname + pid)
    ApMon* mApMon;                        ///< ApMon object
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H
