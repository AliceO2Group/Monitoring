#include <iostream>
#include <string>
#include <boost/program_options.hpp>

std::string GetConfigFromCmdLine(int argc, char *argv[])
{
  std::string config = "/home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini";

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("config", boost::program_options::value<std::string>(), "Config file absolute path")
  ;
  
  boost::program_options::variables_map vm; 
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("config")) {
    config = vm["config"].as<std::string>();
  }
  std::cout << "Configuration file: '/" << config << "'" << std::endl;
  return config;
}
