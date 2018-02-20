///
/// \file 7-Latency.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <boost/program_options.hpp>
#include <random>

using namespace AliceO2::Monitoring;

int main(int argc, char *argv[]) {
  int sleep = 10000;
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("sleep", boost::program_options::value<int>(), "Thread sleep in microseconds")
    ("url", boost::program_options::value<std::string>()->required(), "URL to monitoring backend (or list of comma seperated URLs)")
  ;

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("sleep")) {
    sleep = vm["sleep"].as<int>();
  }

  auto monitoring = MonitoringFactory::Get(vm["url"].as<std::string>());

  for (;;) {
    auto timestamp = std::chrono::system_clock::now();
    monitoring->send(
      Metric{999, "latencyTestMetric"}.addTags(
        {{"libraryTimestamp", std::to_string(
          std::chrono::duration_cast <std::chrono::nanoseconds>(timestamp.time_since_epoch()).count()
        )}}
      )
    );
    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
  }
}	
