///
/// \file 12-KafkaToInfluxDb.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "../src/Transports/KafkaConsumer.h"
#include "../src/Backends/InfluxDB.h"
#include "../src/Transports/HTTP.h"

#include <iostream>
#include <memory>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

#include "envs.pb.h"

using namespace o2::monitoring;

int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka broker hostname")
    ("kafka-topics", boost::program_options::value<std::vector<std::string>>()->multitoken()->required(), "Kafka topics")
    ("influxdb-host", boost::program_options::value<std::string>()->required(), "InfluxDB hostname")
    ("influxdb-token", boost::program_options::value<std::string>()->required(), "InfluxDB token")
    ("influxdb-org", boost::program_options::value<std::string>()->default_value("cern"), "InfluxDB organisation")
    ("influxdb-bucket", boost::program_options::value<std::string>()->default_value("aliecs"), "InfluxDB bucket");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  std::vector<std::string> topics = vm["kafka-topics"].as<std::vector<std::string>>();
  auto kafkaConsumer = std::make_unique<transports::KafkaConsumer>(vm["kafka-host"].as<std::string>(), 9092, topics);
  auto httpTransport = std::make_unique<transports::HTTP>(
    "http://" + vm["influxdb-host"].as<std::string>() + ":8086/api/v2/write?" +
    "org=" + vm["influxdb-org"].as<std::string>() + "&" +
    "bucket=" + vm["influxdb-bucket"].as<std::string>()
  );
  httpTransport->addHeader("Authorization: Token " + vm["influxdb-token"].as<std::string>());
  auto influxdbBackend = std::make_unique<backends::InfluxDB>(std::move(httpTransport));
  for (;;) {
    auto changes = kafkaConsumer->receive();
    if (!changes.empty()) {
      for (auto& change : changes) {
        aliceo2::envs::NewStateNotification stateChange;
        stateChange.ParseFromString(change);
        if (stateChange.envinfo().state().empty()) {
          continue;
        }
        std::cout << stateChange.envinfo().environmentid() << " => " << stateChange.envinfo().state()
                  << " (" << stateChange.envinfo().runnumber() << ")" << std::endl;
        auto metric = Metric{"env_info"}.addValue(stateChange.envinfo().state(), "state");
        auto detectorsProto = stateChange.envinfo().detectors();
        std::vector<std::string> detectors(detectorsProto.begin(), detectorsProto.end());
        if (detectors.size() > 0) {
          metric.addValue(boost::algorithm::join(detectors, " "), "detectors");
        }
        int run = stateChange.envinfo().runnumber();
        if (run > 1) {
          metric.addValue(run, "run");
        }
        influxdbBackend->sendWithId(metric, stateChange.envinfo().environmentid());
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
