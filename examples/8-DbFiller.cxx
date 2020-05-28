///
/// \file 8-DbFiller.cxx
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
    ("url", boost::program_options::value<std::string>()->required(), "URL to monitoring backend (or list of comma seperated URLs)")
    ("measurements", boost::program_options::value<int>()->default_value(1), "Number of different measurements")
    ("flps", boost::program_options::value<int>()->default_value(1), "Number of FLPs")
    ("since", boost::program_options::value<int>()->default_value(60), "Start filling since (s)")
    ("interval", boost::program_options::value<int>()->default_value(1), "Interval between metrics (s)");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  auto monitoring = MonitoringFactory::Get(vm["url"].as<std::string>());
  monitoring->addGlobalTag(tags::Key::Subsystem, tags::Value::QC);

  auto now = Metric::getCurrentTimestamp();
  auto interval = std::chrono::seconds(vm["interval"].as<int>());
  auto since = now - std::chrono::seconds(vm["since"].as<int>());

  for (;;) {
    since = since + interval;
    for (int i = 1; i <= vm["measurements"].as<int>(); i++) {
      for (int k = 1; k <= vm["flps"].as<int>(); k++) {
        auto metric = Metric{"metric" + std::to_string(i), Metric::DefaultVerbosity, since}
          .addValue(doubleDist(mt), "double")
          .addValue(intDist(mt), "int")
          .addValue(std::rand() % 2, "onOff")
          .addTag(tags::Key::FLP, k);
        monitoring->send(std::move(metric));
        std::this_thread::sleep_for(std::chrono::microseconds(1));
      }
    }
    if (since > now) break;
  }
}
