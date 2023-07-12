///
/// \file 14-OrbitId.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "../src/Transports/KafkaConsumer.h"
#include "../src/Transports/HTTP.h"
#include "../src/MonLogger.h"

#include <iostream>
#include <memory>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

#include "envs.pb.h"

using namespace o2::monitoring;

std::map<std::string, unsigned int> detectorRunMap;

std::map<unsigned int, std::string> referenceOrbitIdMap;

std::string getValueFromMetric(const std::string& key, const std::string& metric) {
  auto indexStart = metric.find(key + "=");
  if (indexStart == std::string::npos) {
    return {};
  }
  auto indexEnd = std::find_if(metric.begin() + indexStart, metric.end(), [](const char& s) { return s == ' ' or s == ','; });
  return std::string(metric.begin() + indexStart + key.size() + 1, indexEnd);
}


int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka broker hostname")
    ("influxdb-url", boost::program_options::value<std::string>()->required(), "InfluxDB hostname")
    ("influxdb-token", boost::program_options::value<std::string>()->required(), "InfluxDB token")
    ("influxdb-org", boost::program_options::value<std::string>()->default_value("cern"), "InfluxDB organisation")
    ("influxdb-bucket", boost::program_options::value<std::string>()->default_value("telegraf"), "InfluxDB bucket");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  std::vector<std::string> topics = {"aliecs.env_list.RUNNING", "cru.link_status"};
  auto kafkaConsumer = std::make_unique<transports::KafkaConsumer>(vm["kafka-host"].as<std::string>() + ":9092", topics, "orbitid");
  /*auto httpTransport = std::make_unique<transports::HTTP>(
    "http://" + vm["influxdb-url"].as<std::string>() + "/api/v2/write?" +
    "org=" + vm["influxdb-org"].as<std::string>() + "&" +
    "bucket=" + vm["influxdb-bucket"].as<std::string>()
  );*/
  auto httpTransport = std::make_unique<transports::HTTP>("http://pcald24.cern.ch:8086/api/v2/write?org=cern&bucket=telegraf");
  //httpTransport->addHeader("Authorization: Token " + vm["influxdb-token"].as<std::string>());
  MonLogger::mLoggerSeverity = Severity::Debug;
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
              auto detector = activeRuns.activeruns(i).detectors(j);
              for (auto& c : detector) c = std::tolower(c);
              detectorRunMap.insert({detector, run});
            }
          }
          if (detectorRunMap.empty()) {
            MonLogger::Get() << "No ongoing runs" << MonLogger::End();
            referenceOrbitIdMap.clear();
          }
          for (const auto &p : detectorRunMap) {
            MonLogger::Get() << p.first << " belongs to run " <<  p.second << MonLogger::End();
          }
          // if SOR
        // handle link status messages
        } else if (message.first == "cru.link_status") {
          auto detector = getValueFromMetric("detector", message.second);
          auto orbitId = getValueFromMetric("orbitSor", message.second);
          auto status = getValueFromMetric("status", message.second);
          if (detector.empty() or orbitId.empty()) {
            continue;
          }
          // if detector is not running
          auto detectorInRun = detectorRunMap.find(detector);
          if (detectorInRun == detectorRunMap.end()) {
            continue;
          }
          // if link is excluded
          if (status != "1i") {
            continue;
          }
          auto referenceOrbit = referenceOrbitIdMap.find(detectorRunMap.at(detector));
          if (referenceOrbit == referenceOrbitIdMap.end()) {
            /// wait for trigger
            if (orbitId == "0i") {
              continue;
            }
            referenceOrbitIdMap.insert({detectorRunMap.at(detector), orbitId});
            MonLogger::Get() << "Set reference orbitId for run " << detectorRunMap.at(detector) << ": " << orbitId << MonLogger::End();
          }
          auto referenceOrbitId = referenceOrbitIdMap.at(detectorRunMap.at(detector));
          if (orbitId != referenceOrbitId) {
             MonLogger::Get() << "Abnormal condition for " << detector << "; expected orbitID: " << referenceOrbitId << " but got: " << orbitId << MonLogger::End();
             std::string outputMetric = message.second.substr(message.second.find(","), message.second.find(" ") - message.second.find(","));
             //httpTransport->send("orbitIdMismatch" + outputMetric + " actual=" + orbitId + " expected=" + referenceOrbitId);
          }
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
