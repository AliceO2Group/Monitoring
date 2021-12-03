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

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

KafkaConsumer::KafkaConsumer(const std::string& host, unsigned int port, const std::string& topic) : mTopic(topic)
{
  std::string errstr;
  std::unique_ptr<RdKafka::Conf> conf{RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL)};
  conf->set("bootstrap.servers", host + ":" + std::to_string(port), errstr);
  conf->set("enable.partition.eof", "false", errstr);
  conf->set("group.id", "o2-monitoring-group", errstr);
  conf->set("auto.offset.reset", "latest", errstr);

  mConsumer = RdKafka::KafkaConsumer::create(conf.get(), errstr);
  if (!mConsumer) {
    MonLogger::Get(Severity::Warn) << "Could not initialize Kafka consumer" << MonLogger::End();
  }
  if (mConsumer->subscribe({mTopic})) {
    MonLogger::Get(Severity::Warn) << "Failed to subscribe to topic" << MonLogger::End();
  }
}

std::vector<std::string> KafkaConsumer::receive()
{
  std::vector<std::string> received;
  size_t batch_size = 5;
  int remaining_timeout = 1000;
  auto start = std::chrono::high_resolution_clock::now();

  while (received.size() < batch_size) {
    std::unique_ptr<RdKafka::Message> message{mConsumer->consume(remaining_timeout)};
    switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
      break;
    case RdKafka::ERR_NO_ERROR:
      received.push_back(std::string(static_cast<char*>(message->payload()), message->len()));
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
