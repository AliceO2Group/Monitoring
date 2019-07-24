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
/// \file Noop.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_NOOP_H
#define ALICEO2_MONITORING_BACKENDS_NOOP_H

#include "Monitoring/Backend.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief No-op backend
///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
class Noop final : public Backend
{
 public:
  /// Constructs backend
  Noop() = default;

  /// Default destructor
  ~Noop() = default;

  /// \@param metrics  vector of metrics
  void send(std::vector<Metric>&& /*metrics*/) final {}

  /// \param metric           reference to metric object:
  void send(const Metric& /*metric*/) final {}

  /// \param measurement     measurement name
  /// \param metrics  list of metrics
  void sendMultiple(std::string /*measurement*/, std::vector<Metric>&& /*metrics*/) final {}

  /// \param name             tag name
  /// \param value            tag value that is concatenated to entity string
  void addGlobalTag(std::string_view /*name*/, std::string_view /*value*/) final {}
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_NOOP_H
