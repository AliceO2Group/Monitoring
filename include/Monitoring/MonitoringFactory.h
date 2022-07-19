// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file MonitoringFactory.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_FACTORY_H
#define ALICEO2_MONITORING_FACTORY_H

#include "Monitoring/Monitoring.h"
#include "Monitoring/PullClient.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Monitoring and monitoring backends factory
class MonitoringFactory
{
 public:
  /// Disables copy constructor
  MonitoringFactory& operator=(const MonitoringFactory&) = delete;
  MonitoringFactory(const MonitoringFactory&) = delete;

  /// Provides single instance on Monitoring Monitoring (singleton)
  /// \return              renerence to Monitoring instance
  /// \throw MonitoringException when backend initialisation failed
  static std::unique_ptr<Monitoring> Get(std::string urlsString) noexcept(false);

  /// Provides a Monitoring backend based on the URL
  /// \return		monitoring backend
  /// \throw MonitoringException when backend initialisation failed
  static std::unique_ptr<Backend> GetBackend(std::string& url);
  static std::unique_ptr<PullClient> GetPullClient(const std::string &url, const std::vector<std::string>& topics, const std::string &label = "o2-monitoring-group");
 private:
  /// Private constructor disallows to create instance of Factory
  MonitoringFactory() = default;

  /// Sets backend verbosity based on the URL path
  static void SetVerbosity(std::string selected, std::unique_ptr<Backend>& backend);
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_FACTORY_H
