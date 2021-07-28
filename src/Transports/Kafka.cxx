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

Kafka::Kafka(const std::string& host, unsigned int port, const std::string& topic)
{
  topic.length() > 0 ? mTopic = topic : mTopic = "test";
  std::string errstr;
  RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  conf->set("bootstrap.servers", host + ":" + std::to_string(port), errstr);
  conf->set("request.required.acks", "0", errstr);
  conf->set("message.send.max.retries", "0", errstr);
  conf->set("queue.buffering.max.ms", "100", errstr);
  conf->set("batch.num.messages", "1000", errstr);

  producer = RdKafka::Producer::create(conf, errstr);
  if (!producer) {
    MonLogger::Get(Severity::Error) << "Coult not initialize Kafka transport" << MonLogger::End();
  }

  MonLogger::Get(Severity::Info) << "Kafka transport initialized (" << host << ":" << port << "/" << mTopic << ")" << MonLogger::End();
}

Kafka::~Kafka()
{
  delete producer;
}

void Kafka::send(std::string&& message)
{
  int32_t partition = RdKafka::Topic::PARTITION_UA;

  RdKafka::ErrorCode resp = producer->produce(
    mTopic, partition,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(message.c_str()), message.size(),
    NULL, 0,
    0,
    NULL,
    NULL);
  if (resp != RdKafka::ERR_NO_ERROR) {
    MonLogger::Get() << "Kafka send failed: " << RdKafka::err2str(resp) << MonLogger::End();
  }
  producer->poll(0);
}

} // namespace transports
} // namespace monitoring
} // namespace o2
