/// \file    DataCollector.cxx
/// \brief   Implementation of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include "Monitoring/DataCollector.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {

DataCollector::DataCollector() {
  setHostname();
  setProcessUniqueId();
}

DataCollector::~DataCollector() {}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, int value)
{
  std::cout << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, double value)
{
	std::cout << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, std::string value)
{
	std::cout << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, int value, int timestamp)
{
	std::cout << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, double value,
    int timestamp)
{
	std::cout << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, std::string value,
    int timestamp)
{
	std::cout << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << std::endl;
}

std::string& DataCollector::getHostname()
{
  if (mHostname.empty()) {
    setHostname();
  }

  return mHostname;
}

void DataCollector::setHostname()
{
  // get hostname
  char hostname[255];
  gethostname(hostname, 255);

  // set hostname
  mHostname = std::string(hostname);
}

std::string& DataCollector::getProcessUniqueId()
{
  if (mProcessUniqueId.empty()) {
    setProcessUniqueId();
  }

  return mProcessUniqueId;
}

void DataCollector::setProcessUniqueId()
{
  mProcessUniqueId = getHostname() + "." + boost::lexical_cast<std::string>(getpid());
}

/// Get the current system timestamp in milliseconds
long DataCollector::getCurrentTimestampMilliseconds()
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  ).count();

  //return ms.count();
}


} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
