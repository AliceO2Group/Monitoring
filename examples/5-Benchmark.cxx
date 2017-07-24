///
/// \file 5-Benchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <boost/program_options.hpp>
#include <random>

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

int main(int argc, char *argv[]) {
  int sleep = 1000000;
  int count = 0;

  std::random_device rd;
  std::mt19937 mt(rd());

  std::uniform_real_distribution<double> doubleDist(1.0, 100.0);
  std::uniform_int_distribution<> intDist(1, 100);

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("sleep", boost::program_options::value<int>(), "Thread sleep in microseconds")
    ("config", boost::program_options::value<std::string>()->required(), "Config file path")
    ("id", boost::program_options::value<std::string>(), "Instance ID")
    ("count", boost::program_options::value<int>(), "Number of metric bunches (x3)")
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("sleep")) {
    sleep = vm["sleep"].as<int>();
  }

  if (vm.count("count")) {
    count = vm["count"].as<int>();
  }

  try {
    Monitoring::Configure("file://" + vm["config"].as<std::string>());
  } catch (std::string &e) {
    std::cout << "Configuration file not found.\n" << e << std::endl;
    std::exit(EXIT_FAILURE);
  }

  int add = 0;
  if (count != 0) {
    count--;
    add = 1;
  }

  for (int i = 0; i <= count; i += add) {
    Monitoring::Get().send({"string" + std::to_string(intDist(mt)), "stringMetric"});
    Monitoring::Get().send({doubleDist(mt), "doubleMetric"});
    Monitoring::Get().send({intDist(mt), "intMetric"});
    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
  }
}
