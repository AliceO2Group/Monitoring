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

Kafka::Kafka(const std::string& host, unsigned int port) : mInfluxDB()
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
  std::string topic_str = "test";
  std::string influxLine = mInfluxDB.toInfluxLineProtocol(metric);
  int32_t partition = RdKafka::Topic::PARTITION_UA;

  RdKafka::ErrorCode resp = producer->produce(
    topic_str, partition,
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
  producer->poll(1);
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
