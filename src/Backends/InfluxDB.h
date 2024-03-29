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
/// \file InfluxDB.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
#define ALICEO2_MONITORING_BACKENDS_INFLUXDB_H

#include "Monitoring/Backend.h"
#include "../Transports/TransportInterface.h"
#include "../MonLogger.h"
#include <chrono>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

/// \brief Backend that sends metrics to InfluxDB time-series databse
///
/// Metrics are converted into Influx Line protocol and then sent via one of available transports
class InfluxDB final : public Backend
{
 public:
  /// Constuctor
  /// \param transport 	Any available transport (udp, unix, kafka)
  InfluxDB(std::unique_ptr<transports::TransportInterface> transport);

  /// Default destructor
  ~InfluxDB() = default;

  /// Convert timestamp to unsigned long as required by InfluxDB
  /// \param timestamp  chrono time_point timestamp
  /// \return  	 timestamp in nanoseconds
  inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

  /// Reimplements setting run number and some transport needs to be notified about that as well
  /// \param runNumber run number to be set
  virtual void setRunNumber(uint32_t runNumber) override;

  /// Sends metric to InfluxDB instance via one transport
  /// \param metric    reference to metric object
  void send(const Metric& metric) override;

  /// Sends metric with run tag
  /// \param metric    reference to metric object
  /// \param id        ID tag value
  void sendWithRun(const Metric& metric, const std::string& envId, const std::string& run);

  /// Sends multiple metrics not related to each other
  /// \@param metrics  vector of metrics
  void send(std::vector<Metric>&& metrics) override;

  /// Adds tag
  /// \param name      tag name
  /// \param value     tag value
  void addGlobalTag(std::string_view name, std::string_view value) override;

  /// Converts metric to Influx Line Protocol format
  /// \param metric
  std::string toInfluxLineProtocol(const Metric& metric);

 private:
  std::unique_ptr<transports::TransportInterface> mTransport; ///< InfluxDB transport
  std::string tagSet;                                         ///< Global tagset (common for each metric)

  /// Escapes " ", "," and "=" characters
  /// \param escaped   string rerference to escape characters from
  void escape(std::string& escaped);
};

} // namespace backends
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_BACKENDS_INFLUXDB_H
