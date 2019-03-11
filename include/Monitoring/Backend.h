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
/// \file Backend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_BACKEND_H
#define ALICEO2_MONITORING_CORE_BACKEND_H

#include <chrono>
#include <string>
#include "Monitoring/Metric.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring 
{

/// \brief Backend pure virtual interface
///
/// Interface that allows to send a metric to remote backend.
/// In addition, default tagset (for all handled metrics) can be created.
class Backend
{
  private:
    /// Verbosity level
    Verbosity verbosityLevel;

  public:
    /// Default constructor
    Backend() { verbosityLevel = Verbosity::Info; }

    /// Default destructor
    virtual ~Backend() = default;

    /// Set verbosity level
    void setVerbosisty(Verbosity level) { verbosityLevel = level; }

    /// Get verbosity level
    Verbosity getVerbosity() { return verbosityLevel; }
	
    /// Sends metric via backend
    virtual void send(const Metric& metric) = 0;

    /// Sends multiple metrics not related to each other
    virtual void send(std::vector<Metric>&& metrics) = 0;

    /// Sends multiple related to each other metrics under a common name
    /// If not supported by backend, falls back into sending single metrics
    virtual void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) = 0;

    /// Sets a tag
    virtual void addGlobalTag(std::string_view name, std::string_view value) = 0;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_BACKEND_H
