/// \file    sampleCollectorStdout.cxx
/// \brief   Example of a data collector process using the DataCollector class
///
/// \author  Vasco Barroso, CERN

#include "Monitoring/DataCollector.h"
#include "Monitoring/Version.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace program_options = boost::program_options;
namespace Monitoring = AliceO2::Monitoring;

using AliceO2::InfoLogger::InfoLogger;

int main(int argc, char* argv[])
{
  std::string configFile;
  program_options::options_description commandLineOptions("Allowed Options");
  program_options::variables_map optionsValues;
  InfoLogger logger;

  // define options from command line
  commandLineOptions.add_options()
    ("help,h", "Print help message")
    ("version,v", "Show program name/version banner and exit.")
    ("revision", "Print the revision number");

  try {
    // parse command line options
    program_options::store(program_options::parse_command_line(argc, argv, commandLineOptions), optionsValues);
    program_options::notify(optionsValues);

    // check for help
    if (optionsValues.count("help")) {
      logger << commandLineOptions << InfoLogger::endm;
      return EXIT_SUCCESS;
    }

    // check for version
    if (optionsValues.count("version")) {
      logger << "sampleCollectorStdout version " << Monitoring::Core::Version::getString() << InfoLogger::endm;
      return EXIT_SUCCESS;
    }

    // check for revision
    if (optionsValues.count("revision")) {
      logger << "revision : " << Monitoring::Core::Version::getRevision() << InfoLogger::endm;
      return EXIT_SUCCESS;
    }
  }
  catch(program_options::error& e)
  {
    // error with a configuration option
    logger << "ERROR: " << e.what() << InfoLogger::endm;
    logger << commandLineOptions << InfoLogger::endm;
    return EXIT_FAILURE;
  }

  try {
    // create monitoring object
    Monitoring::Core::DataCollector *collector = new Monitoring::Core::DataCollector();

    // send an application specific value every 10 seconds
    while (true) {
      collector->sendValue("FLPs", "FLP-TPC-01", "myCrazyMetric", 10);
      sleep(10);
    }
  }
  catch (std::runtime_error& e) {
    logger << "Runtime error: " << e.what() << InfoLogger::endm;
    return EXIT_FAILURE;
  }
  catch (std::logic_error& e) {
    logger << "Logic error: " << e.what() << InfoLogger::endm;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
