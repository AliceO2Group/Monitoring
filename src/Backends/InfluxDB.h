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
  /// Constructor for UDP transport
  /// \param host      InfluxDB UDP endpoint hostname
  /// \param port      InfluxDB UDP endpoint port number
  InfluxDB(const std::string& host, unsigned int port);

  /// Constructor for other backends
  InfluxDB();

  /// Constructor for Unix socket transport
  InfluxDB(const std::string& socketPath);

  /// Default destructor
  ~InfluxDB() = default;

  /// Convert timestamp to unsigned long as required by InfluxDB
  /// \param timestamp  chrono time_point timestamp
  /// \return  	 timestamp in nanoseconds
  inline unsigned long convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

  /// Sends metric to InfluxDB instance via one transport
  /// \param metric    reference to metric object
  void send(const Metric& metric) override;

  /// Sends multiple metrics not related to each other
  /// \@param metrics  vector of metrics
  void send(std::vector<Metric>&& metrics) override;

  /// Sends multiple values in single measurement
  /// \param measurement measurement name
  /// \param metrics 	list of metrics
  void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) override;

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
