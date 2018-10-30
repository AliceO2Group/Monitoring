///
/// \file StdOut.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKEND_STDOUT_H
#define ALICEO2_MONITORING_BACKEND_STDOUT_H

#include "Monitoring/Backend.h"
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Prints metrics to standard output via std::cout
class StdOut final : public Backend
{
  public:
    /// Default constructor
    StdOut();
 
    /// Default destructor
    ~StdOut() = default;

    /// Prints metric
    /// \param metric           reference to metric object    
    void send(const Metric& metric) override;

    /// Prints vector of metrics
    /// \@param metrics  vector of metrics
    void send(std::vector<Metric>&& metrics) override;

    /// Prints a metric with multiple values (names are prefixed with measurement)
    /// \param measurement   measurement name
    /// \param metrics  list of metrics
    void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

    /// Adds tag
    /// \param name         tag name
    /// \param value        tag value
    void addGlobalTag(std::string name, std::string value) override;

  private:
    /// Metric stream
    std::ostream &mStream;

    /// Converts timestamp to unsigned long (miliseconds from epoch)
    /// \param timestamp    timestamp in std::chrono::time_point format
    /// \return             timestamp as unsigned long (miliseconds from epoch)
    unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

    std::string tagString; ///< Global tagset (common for each metric)
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKEND_STDOUT_H
