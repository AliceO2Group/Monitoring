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
  int remaining_timeout = 1000;
  size_t batch_size = 5;
  std::vector<std::string> msgs;
  msgs.reserve(batch_size);

  auto start = std::chrono::high_resolution_clock::now();

  while (msgs.size() < batch_size) {
    std::unique_ptr<RdKafka::Message> msg{mConsumer->consume(remaining_timeout)};
    
    switch (msg->err()) {
    case RdKafka::ERR__TIMED_OUT:
      return msgs;

    case RdKafka::ERR_NO_ERROR:
      msgs.push_back(std::string(static_cast<char*>(msg->payload()), msg->len()));
      break;

    default:
      std::cerr << "%% Consumer error: " << msg->errstr() << std::endl;
      return msgs;
    }
    auto now = std::chrono::high_resolution_clock::now();
    int remaining_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count(); 
    if (remaining_timeout > 1000) {
      break;
    }
  }

  return msgs;
}

KafkaConsumer::~KafkaConsumer()
{
  delete mConsumer;
}

} // namespace transports
} // namespace monitoring
} // namespace o2
