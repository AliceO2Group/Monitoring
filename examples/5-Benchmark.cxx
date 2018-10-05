///
/// \file 5-Benchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <boost/program_options.hpp>
#include <random>

using Monitoring = o2::monitoring::MonitoringFactory;

int main(int argc, char *argv[]) {
  int sleep = 1000000;
  int count = 1;
  int measurements = 1;

  std::random_device rd;
  std::mt19937 mt(rd());

  std::uniform_real_distribution<double> doubleDist(1.0, 100.0);
  std::uniform_int_distribution<> intDist(1, 100);

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("sleep", boost::program_options::value<int>(), "Thread sleep in microseconds")
    ("url", boost::program_options::value<std::string>()->required(), "URL to monitoring backend (or list of comma seperated URLs)")
    ("id", boost::program_options::value<std::string>(), "Instance ID")
    ("count", boost::program_options::value<int>(), "Number of loop cycles")
    ("multiple", boost::program_options::bool_switch()->default_value(false), "Sends multiple metrics per measurement")
    ("vector", boost::program_options::bool_switch()->default_value(false), "Sends vector of metrics")
    ("monitor", boost::program_options::bool_switch()->default_value(false), "Enabled process monitor")
    ("buffer", boost::program_options::value<int>(), "Creates buffr of given size")
    ("measurements", boost::program_options::value<int>(), "Number of different measurements") 
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

  if (vm.count("measurements")) {
    measurements = vm["measurements"].as<int>();
  }

  auto monitoring = Monitoring::Get(vm["url"].as<std::string>());
  if (vm["monitor"].as<bool>()) {
    monitoring->enableProcessMonitoring(1);
  }
  if (vm["multiple"].as<bool>()) {
    for (int j = 1; j <= count; j++) {
      for (int i = 1; i <= measurements; i++) {
        monitoring->sendGrouped("measurement" + std::to_string(i) ,{
          {doubleDist(mt), "doubleMetric"  + std::to_string(i)},
          {intDist(mt), "intMetric"  + std::to_string(i)}
        });
        std::this_thread::sleep_for(std::chrono::microseconds(sleep));
      }
      if (!vm.count("count")) j--;
    }
  } else if (vm["vector"].as<bool>()) {
    for (int j = 1; j <= count; j++) {
      for (int i = 1; i <= measurements; i++) {
        monitoring->send({
          {doubleDist(mt), "doubleMetric"  + std::to_string(i)},
          {intDist(mt), "intMetricDebug"  + std::to_string(i)}
        });
        std::this_thread::sleep_for(std::chrono::microseconds(sleep));
      }
      if (!vm.count("count")) j--;
    }
  } else {
    if (vm.count("buffer")) {
      monitoring->enableBuffering(vm["buffer"].as<int>());
    }
    for (int j = 1; j <= count; j++) {
      for (int i = 1; i <= measurements; i++) {
        monitoring->send({doubleDist(mt), "doubleMetric"  + std::to_string(i)});
        monitoring->send({intDist(mt), "intMetric" + std::to_string(i)});
        monitoring->debug({intDist(mt), "intMetricDebug" + std::to_string(i)});
        std::this_thread::sleep_for(std::chrono::microseconds(sleep));
      }
      if (!vm.count("count")) j--;
    }
  }
}
