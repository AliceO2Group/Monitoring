///
/// \file 1-Basic.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Monitoring/Collector.h"

namespace Monitoring = AliceO2::Monitoring;

int main(int argc, char *argv[]) {
  int sleep = 1;
  int i = std::numeric_limits<int>::max() - 1;
  std::string metric = "myCrazyMetric";

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("metric", boost::program_options::value<std::string>(), "metric name")
    ("sleep", boost::program_options::value<int>(), "thread sleep in microseconds")
    ("count", boost::program_options::value<int>(), "Number of metrics to be sent")
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
  
  // create monitoring object, pass confuguration path as parameter
  std::unique_ptr<Monitoring::Core::Collector> collector(
    new Monitoring::Core::Collector("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini")
  );

  for (; i > 0; i--) {
    collector->send(10, metric);
    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
  }
}	
