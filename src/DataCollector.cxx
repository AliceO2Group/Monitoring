/// \file    DataCollector.cxx
/// \brief   Implementation of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#include "Monitoring/DataCollector.h"
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <string>

namespace AliceO2 {
namespace Monitoring {
namespace Core {

using AliceO2::InfoLogger::InfoLogger;

DataCollector::DataCollector() {
  setHostname();
  setProcessUniqueId();
}

DataCollector::~DataCollector() {}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, int value)
{
  getLogger() << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, double value)
{
  getLogger() << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, std::string value)
{
  getLogger() << getCurrentTimestampMilliseconds() << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, int value, int timestamp)
{
  getLogger() << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, double value,
    int timestamp)
{
  getLogger() << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, std::string value,
    int timestamp)
{
  getLogger() << timestamp << "\t" << cluster << "\t" << node << "\t" << metric << "\t" << value << InfoLogger::endm;
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

long DataCollector::getCurrentTimestampMilliseconds()
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  ).count();

  //return ms.count();
}

InfoLogger& DataCollector::getLogger()
{
  return mLogger;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
