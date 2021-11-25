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
/// \file Kafka.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Kafka.h"
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

Kafka::Kafka(const std::string& host, unsigned int port, const std::string& topic, bool isProducer) : mTopic(topic)
{
  std::string errstr;
  std::unique_ptr<RdKafka::Conf> conf{RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL)};
  conf->set("bootstrap.servers", host + ":" + std::to_string(port), errstr);
  conf->set("request.required.acks", "0", errstr);
  conf->set("message.send.max.retries", "0", errstr);
  conf->set("queue.buffering.max.ms", "100", errstr);
  conf->set("batch.num.messages", "1000", errstr);

  if (isProducer) {
    mProducer = RdKafka::Producer::create(conf.get(), errstr);
    if (!mProducer) {
      MonLogger::Get(Severity::Warn) << "Could not initialize Kafka producer" << MonLogger::End();
    }
    MonLogger::Get(Severity::Info) << "Kafka transport initialized (" << host << ":" << port << "/" << mTopic << ")" << MonLogger::End();
  } else {
    mConsumer = RdKafka::KafkaConsumer::create(conf.get(), errstr);
    if (!mConsumer) {
      MonLogger::Get(Severity::Warn) << "Could not initialize Kafka consumer" << MonLogger::End();
    }
    if (mConsumer->subscribe({mTopic})) {
      MonLogger::Get(Severity::Warn) << "Failed to subscribe to topic" << MonLogger::End();
    }
  }
}

std::vector<std::string> Kafka::receive()
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

Kafka::~Kafka()
{
  delete mProducer;
}

void Kafka::send(std::string&& message)
{
  int32_t partition = RdKafka::Topic::PARTITION_UA;

  RdKafka::ErrorCode resp = mProducer->produce(
    mTopic, partition,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(message.c_str()), message.size(),
    NULL, 0,
    0,
    NULL,
    NULL);
  if (resp != RdKafka::ERR_NO_ERROR) {
    MonLogger::Get(Severity::Warn) << "Kafka send failed: " << RdKafka::err2str(resp) << MonLogger::End();
  }
  mProducer->poll(0);
}

} // namespace transports
} // namespace monitoring
} // namespace o2
