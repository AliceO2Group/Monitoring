/// \file    DataCollector.h
/// \brief   Definition of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#ifndef ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H
#define ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H

#include <string>
#include "Monitoring/DataCollectorInterface.h"

namespace AliceO2 {
/// ALICE O2 Monitoring system
namespace Monitoring {
/// Core features of ALICE O2 Monitoring system
namespace Core {
/// Interface to Monitoring system
///
/// Base class of Monitoring system, allowing any process
/// to send application-specific values
class DataCollector : public DataCollectorInterface
{
  public:
    /// Standard constructor
    DataCollector();

    /// Destructor
    virtual ~DataCollector();

    /// Send integer value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    virtual void sendValue(std::string cluster, std::string node, std::string metric, int value);

    /// Send double value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    virtual void sendValue(std::string cluster, std::string node, std::string metric, double value);

    /// Send string value
    /// \param  cluster  Cluster name
    /// \param  node     Node name
    /// \param  metric   Metric name
    /// \param  value    Metric value
    virtual void sendValue(std::string cluster, std::string node, std::string metric, std::string value);

    /// Send timed integer value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    virtual void sendTimedValue(std::string cluster, std::string node, std::string metric, int value, int timestamp);

    /// Send timed double value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    virtual void sendTimedValue(std::string cluster, std::string node, std::string metric, double value, int timestamp);

    /// Send timed string value
    ///
    /// This function can be used to send values measured in the past
    /// \param  cluster    Cluster name
    /// \param  node       Node name
    /// \param  metric     Metric name
    /// \param  value      Metric value
    /// \param  timestamp  Timestamp of measurement in seconds
    virtual void sendTimedValue(std::string cluster, std::string node, std::string metric, std::string value, int timestamp);

  protected:
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

    /// Get the current system timestamp in milliseconds
    long getCurrentTimestampMilliseconds();

    std::string mHostname;                ///< Hostname where process is running
    std::string mProcessUniqueId;         ///< Identifier for process metrics (hostname + pid)
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DATA_COLLECTOR_H
