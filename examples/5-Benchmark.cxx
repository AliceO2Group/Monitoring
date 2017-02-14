///
/// \file 5-Benchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main(int argc, char *argv[]) {
  int sleep = 1000000;
  int i = std::numeric_limits<int>::max() - 1;
  std::string metric = "myCrazyMetric";
  std::string config = "SampleConfig.ini";

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("metric", boost::program_options::value<std::string>(), "Metric name")
    ("sleep", boost::program_options::value<int>(), "Thread sleep in microseconds")
    ("count", boost::program_options::value<int>(), "Number of metrics to be sent")
    ("config", boost::program_options::value<std::string>(), "Config file")
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("metric")) {
    metric = vm["metric"].as<std::string>();
  }

  if (vm.count("sleep")) {
    sleep = vm["sleep"].as<int>();
  }

  if (vm.count("count")) {
    i = vm["count"].as<int>();
  }

  if (vm.count("config")) {
    config = vm["config"].as<std::string>();
  }
  
  // create monitoring object, pass confuguration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/" + config)
  );

  for (; i > 0; i--) {
    collector->send(10, metric);
    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
  }
}	
