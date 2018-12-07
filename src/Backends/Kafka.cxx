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

Kafka::Kafka(const std::string& host, unsigned int port)
{
  std::string errstr;
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  std::string brokers = "localhost:1234";
  conf->set("metadata.broker.list", brokers, errstr);
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

void Kafka::sendMultiple(std::string measurement, std::vector<Metric>&& metrics)
{
}

void Kafka::send(std::vector<Metric>&& metrics)
{
}

void Kafka::send(const Metric& metric)
{
  std::string topic_str = "metric";
  std::string name = metric.getName();
  int32_t partition = RdKafka::Topic::PARTITION_UA;
  RdKafka::Headers *headers = RdKafka::Headers::create();
  headers->add("timestamp", convertTimestamp(metric.getTimestamp()));
  headers->add("value", boost::lexical_cast<std::string>(metric.getValue()));

  for (const auto& tag : globalHeader) {
    headers->add("tag_" + tag.first, tag.second);
  }

  RdKafka::ErrorCode resp = producer->produce(
    topic_str, partition,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(name.c_str()), name.size(),
    NULL, 0,
    0,  
    headers,
    NULL
  );  
  if (resp != RdKafka::ERR_NO_ERROR) {
    MonLogger::Get() << "% Produce failed: " << RdKafka::err2str(resp) << MonLogger::End();
    delete headers;
  } else {
    MonLogger::Get() << "% Produced message (" << name.size() << " bytes)" << MonLogger::End();
  }
  //producer->poll(1);
  producer->flush(100);
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
