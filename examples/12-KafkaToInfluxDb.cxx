///
/// \file 12-KafkaToInfluxDb.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "../src/Transports/KafkaConsumer.h"
#include "../src/Transports/HTTP.h"
#include "../src/Transports/WebSocket.h"

#include <iostream>
#include <memory>
#include <thread>
#include <boost/algorithm/string/join.hpp>
#include <boost/program_options.hpp>

#include "envs.pb.h"

using namespace o2::monitoring;

int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka broker hostname")
    ("kafka-topic", boost::program_options::value<std::string>()->required(), "Kafka topic");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  auto kafkaConsumer = std::make_unique<transports::KafkaConsumer>(vm["kafka-host"].as<std::string>(), 9092, vm["kafka-topic"].as<std::string>());
  for (;;) {
    auto changes = kafkaConsumer->receive();
    if (!changes.empty()) {
      for (auto& change : changes) {
        aliceo2::envs::NewStateNotification stateChange;
        stateChange.ParseFromString(change);
        std::cout << "EnvID: " << stateChange.envinfo().environmentid() << std::endl
                  << "State: " << stateChange.envinfo().state() << std::endl
                  << "First detector: " << stateChange.envinfo().detectors().Get(0) << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
