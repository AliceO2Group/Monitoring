///
/// \file 5-Benchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <boost/program_options.hpp>
#include <random>

using o2::monitoring::Metric;
using namespace o2::monitoring;

int main(int argc, char* argv[])
{
  std::srand(std::time(nullptr));
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> doubleDist(1.0, 100.0);
  std::uniform_int_distribution<> intDist(1, 100);

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("sleep", boost::program_options::value<int>()->default_value(1000000), "Thread sleep in microseconds")
    ("url", boost::program_options::value<std::string>()->required(), "URL to monitoring backend (or list of comma seperated URLs)")
    ("count", boost::program_options::value<int>()->default_value(1), "Number of loop cycles")
    ("multiple", boost::program_options::bool_switch()->default_value(false), "Sends multiple metrics per measurement")
    ("latency", boost::program_options::bool_switch()->default_value(false), "Sends timestamp as a value")
    ("monitor", boost::program_options::bool_switch()->default_value(false), "Enabled process monitor")
    ("buffer", boost::program_options::value<int>(), "Creates buffr of given size")
    ("measurements", boost::program_options::value<int>()->default_value(1), "Number of different measurements")
    ("flps", boost::program_options::value<int>()->default_value(1), "Number of FLPs")
    ("crus", boost::program_options::value<int>()->default_value(1), "Number of CRUss (optional)");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  int flps = vm["flps"].as<int>();
  int crus = vm["crus"].as<int>();
  int sleep = vm["sleep"].as<int>();
  int count = vm["count"].as<int>();
  int measurements = vm["measurements"].as<int>();

  auto monitoring = MonitoringFactory::Get(vm["url"].as<std::string>());
  if (vm["monitor"].as<bool>()) {
    monitoring->enableProcessMonitoring(1);
  }
  if (vm["multiple"].as<bool>()) {
    for (int j = 1; j <= count; j++) {
      for (int i = 1; i <= measurements; i++) {
        for (int k = 1; k <= flps; k++) {
          for (int l = 1; l <= crus; l++) {
            monitoring->send(Metric{"measurement" + std::to_string(i)}
              .addValue(doubleDist(mt), "double_metric")
              .addValue(intDist(mt), "int_metric")
              .addValue(std::rand() % 2, "on_off_metric")
              .addTag(tags::Key::FLP, k)
              .addTag(tags::Key::CRU, l)
            );
          }
          std::this_thread::sleep_for(std::chrono::microseconds(sleep));
        }
      }
      if (!vm.count("count"))
        j--;
    }
  } else if (vm["latency"].as<bool>()) {
    Metric::includeTimestamp = false;
    for (int j = 1; j <= count; j++) {
      auto timestamp = Metric::getCurrentTimestamp();
      uint64_t nowTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
           timestamp.time_since_epoch())
          .count();
      monitoring->send({nowTimestamp, "latency"});
      std::this_thread::sleep_for(std::chrono::microseconds(sleep));
      if (!vm.count("count"))
        j--;
    }
  } else {
    if (vm.count("buffer")) {
      monitoring->enableBuffering(vm["buffer"].as<int>());
    }
    for (int j = 1; j <= count; j++) {
      for (int i = 1; i <= measurements; i++) {
        for (int k = 1; k <= flps; k++) {
          for (int l = 1; l <= crus; l++) {
            monitoring->send(Metric{doubleDist(mt), "doubleMetric" + std::to_string(i)}.addTag(tags::Key::FLP, k).addTag(tags::Key::CRU, l));
            monitoring->send(Metric{intDist(mt), "intMetric" + std::to_string(i)}.addTag(tags::Key::FLP, k).addTag(tags::Key::CRU, l));
            monitoring->send(Metric{std::rand() % 2, "onOffMetric" + std::to_string(i)}.addTag(tags::Key::FLP, k).addTag(tags::Key::CRU, l));
            std::this_thread::sleep_for(std::chrono::microseconds(10));
          }
          std::this_thread::sleep_for(std::chrono::microseconds(sleep));
        }
      }
      if (!vm.count("count"))
        j--;
    }
  }
}
