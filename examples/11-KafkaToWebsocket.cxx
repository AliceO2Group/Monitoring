///
/// \file 11-KafkaToHttp.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "../src/Transports/HTTP.h"
#include "../src/Transports/WebSocket.h"

#include <iostream>
#include <memory>
#include <thread>
#include <boost/program_options.hpp>

using namespace o2::monitoring;

int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka broker hostname")
    ("kafka-topics", boost::program_options::value<std::vector<std::string>>()->multitoken()->required(), "Kafka topics")
    ("grafana-host", boost::program_options::value<std::string>()->required(), "Grafana hostname")
    ("grafana-key", boost::program_options::value<std::string>()->required(), "Grafana API key");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);


  auto kafkaConsumer = MonitoringFactory::GetPullClient(vm["kafka-host"].as<std::string>() + ":9092", std::vector<std::string>{vm["kafka-topics"].as<std::vector<std::string>>()});
  auto outTransport = std::make_unique<transports::WebSocket>(vm["grafana-host"].as<std::string>(), 3000, vm["grafana-key"].as<std::string>(), "alice_o2");
  std::thread readThread([&outTransport](){
    for (;;) {
      outTransport->read();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });
  for (;;) {
    auto metrics = kafkaConsumer->pull();
    for (auto& metric : metrics) {
      outTransport->send(std::move(metric.second));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
