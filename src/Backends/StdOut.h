// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file StdOut.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKEND_STDOUT_H
#define ALICEO2_MONITORING_BACKEND_STDOUT_H

#include "Monitoring/Backend.h"
#include <string>
#include <sstream>

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
  StdOut(const std::string& prefix = "METRIC");

  /// Default destructor
  ~StdOut() = default;

  /// Prints metric
  /// \param metric           reference to metric object
  void send(const Metric& metric) override;

  /// Prints vector of metrics
  /// \@param metrics  vector of metrics
  void send(std::vector<Metric>&& metrics) override;

  /// Adds tag
  /// \param name         tag name
  /// \param value        tag value
  void addGlobalTag(std::string_view name, std::string_view value) override;

 private:
  /// Metric stream
  std::ostringstream mStream;

  /// Converts timestamp to unsigned long (miliseconds from epoch)
  /// \param timestamp    timestamp in std::chrono::time_point format
  /// \return             timestamp as unsigned long (miliseconds from epoch)
  unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

  std::string tagString;     ///< Global tagset (common for each metric)
  const std::string mPrefix; ///< Metric prefix
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKEND_STDOUT_H
