///
/// \file Kafka.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Kafka.h"
#include <string>
#include <boost/lexical_cast.hpp>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring backends
namespace backends
{

Kafka::Kafka(const std::string& host, unsigned int port, const std::string& topic) :
  mInfluxDB(), mTopic(topic)
{
  std::string errstr;
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  conf->set("bootstrap.servers", host + ":" + std::to_string(port), errstr);
  conf->set("request.required.acks", "0", errstr);
  conf->set("message.send.max.retries", "0", errstr);
  conf->set("queue.buffering.max.ms", "10", errstr);
  conf->set("batch.num.messages", "1000", errstr);

  producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      MonLogger::Get() << "Failed to create producer: " << errstr << MonLogger::End();
      exit(1);
    }

  MonLogger::Get() << "Kafka backend initialized"
                   << " ("<< host << ":" << port << ")" << MonLogger::End();
}

Kafka::~Kafka()
{
  delete producer;
}

inline std::string Kafka::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::to_string(std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count());
}

void Kafka::sendMultiple(std::string /*measurement*/, std::vector<Metric>&& /*metrics*/)
{
}

void Kafka::send(std::vector<Metric>&& /*metrics*/)
{
}

void Kafka::send(const Metric& metric)
{
  //std::string influxLine = mInfluxDB.toInfluxLineProtocol(metric);
  int32_t partition = RdKafka::Topic::PARTITION_UA;
  auto timestamp = std::chrono::system_clock::now();
  std::string influxLine = "latency,tagkey1=tagvalue1,tagkey2=tagvalue2 doublefield=1.0,starttime=" + std::to_string(
    std::chrono::duration_cast <std::chrono::nanoseconds>(timestamp.time_since_epoch()).count()
  );
  RdKafka::ErrorCode resp = producer->produce(
    mTopic, partition,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(influxLine.c_str()), influxLine.size(),
    NULL, 0,
    0,  
    NULL,
    NULL
  );  
  if (resp != RdKafka::ERR_NO_ERROR) {
    MonLogger::Get() << "% Produce failed: " << RdKafka::err2str(resp) << MonLogger::End();
  }
  producer->poll(0);
}

void Kafka::addGlobalTag(std::string_view name, std::string_view value)
{
  std::string sName = name.data();
  std::string sValue = value.data();
  if (!tagSet.empty()) tagSet += ",";
  tagSet += sName + "=" + sValue;
}

} // namespace backends
} // namespace monitoring
} // namespace o2
