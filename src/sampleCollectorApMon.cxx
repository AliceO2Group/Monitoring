/// \file    sampleCollectorApMon.cxx
/// \brief   Example of a data collector process
///
/// \author  Vasco Barroso, CERN

#include "Monitoring/DataCollectorApMon.h"
#include "Monitoring/FileNotFoundException.h"
#include "Monitoring/Version.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace program_options = boost::program_options;
namespace Monitoring = AliceO2::Monitoring;

int main(int argc, char* argv[])
{
  std::string configFile;
  program_options::options_description commandLineOptions("Allowed Options");
  program_options::variables_map optionsValues;

  // define options from command line
  commandLineOptions.add_options()
    ("help,h", "Print help message")
    ("version,v", "Show program name/version banner and exit.")
    ("revision", "Print the revision number")
    ("config-file,c", program_options::value<std::string>(&configFile), "Configuration file path");

  try {
    // parse command line options
    program_options::store(program_options::parse_command_line(argc, argv, commandLineOptions), optionsValues);
    program_options::notify(optionsValues);

    // check for help
    if (optionsValues.count("help")) {
      std::cout << commandLineOptions << std::endl;
      return EXIT_SUCCESS;
    }

    // check for version
    if (optionsValues.count("version")) {
      std::cout << "sampleCollectorApMon version " << Monitoring::Core::Version::getString() << std::endl;
      return EXIT_SUCCESS;
    }

    // check for revision
    if (optionsValues.count("revision")) {
      std::cout << "revision : " << Monitoring::Core::Version::getRevision() << std::endl;
      return EXIT_SUCCESS;
    }

    // check ApMon config file
    if (!optionsValues.count("config-file")) {
      throw program_options::error("missing mandatory option --config-file");
    }
  }
  catch(program_options::error& e)
  {
    // error with a configuration option
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << commandLineOptions << std::endl;
    return EXIT_FAILURE;
  }

  try {
    // create monitoring object
    Monitoring::Core::DataCollectorApMon *collector = new Monitoring::Core::DataCollectorApMon(configFile);

    // send an application specific value every 10 seconds
    while (true) {
      collector->sendValue("FLPs", "FLP-TPC-01", "myCrazyMetric", 10);
      sleep(10);
    }
  }
  catch (Monitoring::Core::FileNotFoundException& e) {
    std::cerr << "Error opening ApMon configuration file: '" << e.getFilePath() << "'" << std::endl;
    return EXIT_FAILURE;
  }
  catch (std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (std::logic_error& e) {
    std::cerr << "Logic error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
