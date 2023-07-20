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
#include "../src/MonLogger.h"

using namespace o2::monitoring;

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

  std::vector<std::string> topics = {"aliecs.env_leave_state.RUNNING", "aliecs.env_state.RUNNING"};
  auto kafkaConsumer = std::make_unique<transports::KafkaConsumer>(vm["kafka-host"].as<std::string>() + ":9092", topics, "aliecs-run-times");
  auto httpTransport = std::make_unique<transports::HTTP>(
    vm["influxdb-url"].as<std::string>() + "/api/v2/write?" +
    "org=" + vm["influxdb-org"].as<std::string>() + "&" +
    "bucket=" + vm["influxdb-bucket"].as<std::string>()
  );
  httpTransport->addHeader("Authorization: Token " + vm["influxdb-token"].as<std::string>());
  auto influxdbBackend = std::make_unique<backends::InfluxDB>(std::move(httpTransport));
  for (;;) {
    auto changes = kafkaConsumer->pull();
    if (!changes.empty()) {
      for (auto& change : changes) {
        aliceo2::envs::NewStateNotification stateChange;
        stateChange.ParseFromString(change.second);
        if (stateChange.envinfo().state().empty()) {
          continue;
        }
        auto metric = Metric{"run_times"};
        if (change.first.find("leave") != std::string::npos) {
          metric.addValue(stateChange.timestamp(), "eor");
          MonLogger::Get() << stateChange.envinfo().environmentid() << "/" << stateChange.envinfo().runnumber() << "  " << change.first << " EOR: " << stateChange.timestamp() << MonLogger::End();
        } else {
          metric.addValue(stateChange.envinfo().runtype(), "type").addValue(stateChange.envinfo().enterstatetimestamp(), "sor");
          MonLogger::Get() << stateChange.envinfo().environmentid() << "/" << stateChange.envinfo().runnumber() << "  " << change.first << " SOR: " <<stateChange.envinfo().enterstatetimestamp() << MonLogger::End();
        }
        int run = stateChange.envinfo().runnumber();
        if (run > 1) {
          influxdbBackend->sendWithRun(metric, stateChange.envinfo().environmentid(), std::to_string(run));
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
