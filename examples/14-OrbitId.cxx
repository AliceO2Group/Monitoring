///
/// \file 14-OrbitId.cxx
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

std::map<std::string, uint32_t> detectorRunMap;

std::map<uint32_t, long> referenceOrbitIdMap;

int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka broker hostname")
    ("influxdb-url", boost::program_options::value<std::string>()->required(), "InfluxDB hostname")
    ("influxdb-token", boost::program_options::value<std::string>()->required(), "InfluxDB token")
    ("influxdb-org", boost::program_options::value<std::string>()->default_value("cern"), "InfluxDB organisation")
    ("influxdb-bucket", boost::program_options::value<std::string>()->default_value("aliecs"), "InfluxDB bucket");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  std::vector<std::string> topics = {"aliecs.env_list.RUNNING", "cru.link_status"};
  auto kafkaConsumer = std::make_unique<transports::KafkaConsumer>(vm["kafka-host"].as<std::string>() + ":9092", topics, "orbitid");
  auto httpTransport = std::make_unique<transports::HTTP>(
    vm["influxdb-url"].as<std::string>() + "/api/v2/write?" +
    "org=" + vm["influxdb-org"].as<std::string>() + "&" +
    "bucket=" + vm["influxdb-bucket"].as<std::string>()
  );
  httpTransport->addHeader("Authorization: Token " + vm["influxdb-token"].as<std::string>());
  auto influxdbBackend = std::make_unique<backends::InfluxDB>(std::move(httpTransport));
  // TODO:
  // store refrence orbitid
  // filter orbitid per run (based on list of detectors)
  // verify if orbitid is different
  // output metric: orbitIdMismatch,detector=,run=,link=, sorOrbitId=,mismatchedObrbitId=
  for (;;) {
    auto messages = kafkaConsumer->pull();
    if (!messages.empty()) {
      for (auto& message : messages) {
        // handle active runs messages
        if (message.first == "aliecs.env_list.RUNNING") {
          aliceo2::envs::ActiveRunsList activeRuns;
          activeRuns.ParseFromString(message.second);
          detectorRunMap.clear();
          for (int i = 0; i < activeRuns.activeruns_size(); i++) {
            auto run = activeRuns.activeruns(i).runnumber();
            for (int j = 0; j < activeRuns.activeruns(i).detectors_size(); j++) {
              detectorRunMap.at(activeRuns.activeruns(i).detectors(j)) = run;
            }
          }
        // handle link status messages
        } else if (message.first == "cru.link_status") {

        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
