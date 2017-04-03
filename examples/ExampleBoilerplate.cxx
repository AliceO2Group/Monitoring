#include <iostream>
#include <string>
#include <boost/program_options.hpp>

std::string GetConfigFromCmdLine(int argc, char *argv[])
{
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("config", boost::program_options::value<std::string>()->required(), "Config file absolute path")
  ;
  
  boost::program_options::variables_map vm; 
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  return vm["config"].as<std::string>();
}
