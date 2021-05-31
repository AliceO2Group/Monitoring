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
/// \file Kafka.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_KAFKA_H
#define ALICEO2_MONITORING_TRANSPORTS_KAFKA_H

#include "TransportInterface.h"

#include <chrono>
#include <string>
#include <librdkafka/rdkafkacpp.h>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport that sends string formatted metrics via Kafka
class Kafka : public TransportInterface
{
 public:
  /// Creates producer
  /// \param hostname      Hostname
  /// \param port          Port number
  /// \param topic 	   Kafka topic
  Kafka(const std::string& host, unsigned int port, const std::string& topic = "test");

  /// Deletes producer
  ~Kafka();

  /// Sends metric via Kafka
  /// \param message   r-value string formated
  void send(std::string&& message) override;

 private:
  /// Kafka producer instance
  RdKafka::Producer* producer;

  /// Kafka topic
  std::string mTopic;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_KAFKA_H
