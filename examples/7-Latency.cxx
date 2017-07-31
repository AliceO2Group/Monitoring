///
/// \file 7-Latency.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "ExampleBoilerplate.cxx"
#include <boost/program_options.hpp>
#include <random>

using Monitoring = AliceO2::Monitoring::MonitoringFactory;
using namespace AliceO2::Monitoring;

int main(int argc, char *argv[]) {
  int sleep = 10000;
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("sleep", boost::program_options::value<int>(), "Thread sleep in microseconds")
    ("config", boost::program_options::value<std::string>()->required(), "Config file path")
  ;

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("sleep")) {
    sleep = vm["sleep"].as<int>();
  }

  try {
    Monitoring::Configure("file://" + vm["config"].as<std::string>());
  } catch (std::string &e) {
    std::cout << "Configuration file not found.\n" << e << std::endl;
    std::exit(EXIT_FAILURE);
  }

  for (;;) {
    auto timestamp = std::chrono::system_clock::now();
    Monitoring::Get().send(
      Metric{999, "latencyTestMetric"}.addTags(
        {{"libraryTimestamp", std::to_string(
          std::chrono::duration_cast <std::chrono::nanoseconds>(timestamp.time_since_epoch()).count()
        )}}
      )
    );
    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
  }
}	
