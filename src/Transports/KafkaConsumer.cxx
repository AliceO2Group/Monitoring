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
/// \file KafkaConsumer.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "KafkaConsumer.h"
#include <string>
#include "../MonLogger.h"
#include "../Exceptions/MonitoringException.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

KafkaConsumer::KafkaConsumer(const std::string& url, const std::vector<std::string>& topics, const std::string& groupId) : mTopics(topics)
{
  std::string errstr;
  std::unique_ptr<RdKafka::Conf> conf{RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL)};
  if (conf->set("bootstrap.servers", url, errstr) != RdKafka::Conf::CONF_OK
   || conf->set("enable.partition.eof", "false", errstr) != RdKafka::Conf::CONF_OK
   || conf->set("group.id", groupId, errstr) != RdKafka::Conf::CONF_OK
   || conf->set("auto.offset.reset", "latest", errstr) != RdKafka::Conf::CONF_OK
   || conf->set("heartbeat.interval.ms", "2000", errstr) != RdKafka::Conf::CONF_OK
   || conf->set("session.timeout.ms", "6000", errstr) != RdKafka::Conf::CONF_OK
  ) {
    throw MonitoringException("Kafka Consumer", errstr);
  }

  mConsumer = RdKafka::KafkaConsumer::create(conf.get(), errstr);
  if (!mConsumer) {
    MonLogger::Get(Severity::Error) << "Could not initialize Kafka consumer" << MonLogger::End();
  }
  if (mConsumer->subscribe(mTopics)) {
    MonLogger::Get(Severity::Warn) << "Failed to subscribe to topic" << MonLogger::End();
  }
}

std::vector<std::pair<std::string, std::string>> KafkaConsumer::pull()
{
  std::vector<std::pair<std::string, std::string>> received;
  size_t batch_size = 5;
  int remaining_timeout = 1000;
  auto start = std::chrono::high_resolution_clock::now();

  while (received.size() < batch_size) {
    std::unique_ptr<RdKafka::Message> message{mConsumer->consume(remaining_timeout)};
    switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
      break;
    case RdKafka::ERR_NO_ERROR:
      received.push_back({message->topic_name(), std::string(static_cast<char*>(message->payload()), message->len())});
      break;
    default:
      std::cerr << "%% Consumer error: " << message->errstr() << std::endl;
      return received;
    }
    auto now = std::chrono::high_resolution_clock::now();
    remaining_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    if (remaining_timeout > 1000) {
      break;
    }
  }
  return received;
}

KafkaConsumer::~KafkaConsumer()
{
  if (mConsumer) {
    mConsumer->close();
  }
  delete mConsumer;
}

} // namespace transports
} // namespace monitoring
} // namespace o2
