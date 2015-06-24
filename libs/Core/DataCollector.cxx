/// \file    DataCollector.cxx
/// \brief   Implementation of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#include <Core/DataCollector.h>
#include <Core/FileNotFoundException.h>

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>

namespace AliceO2 {
namespace Monitoring {
namespace Core {

DataCollector::DataCollector(std::string configurationFile)
    : mConfigurationFile(configurationFile)
{
  // create ApMon object
  try {
    mApMon = new ApMon(const_cast<char *>(configurationFile.c_str()));
  } catch (std::runtime_error& e) {
    throw FileNotFoundException(configurationFile);
  }
}

DataCollector::~DataCollector()
{
  delete mApMon;
}

ApMon* DataCollector::getApMon() const
{
  return mApMon;
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, int value)
{
  getApMon()->sendParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), value);
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, double value)
{
  getApMon()->sendParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), value);
}

void DataCollector::sendValue(std::string cluster, std::string node, std::string metric, std::string value)
{
  getApMon()->sendParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), const_cast<char *>(value.c_str()));
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, int value, int timestamp)
{
  getApMon()->sendTimedParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), XDR_INT32, (char *) &value, timestamp);
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, double value,
    int timestamp)
{
  getApMon()->sendTimedParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), XDR_REAL64, (char *) &value, timestamp);
}

void DataCollector::sendTimedValue(std::string cluster, std::string node, std::string metric, std::string value,
    int timestamp)
{
  getApMon()->sendTimedParameter(const_cast<char *>(cluster.c_str()), const_cast<char *>(node.c_str()),
      const_cast<char *>(metric.c_str()), XDR_STRING, const_cast<char *>(value.c_str()), timestamp);
}

void DataCollector::log(int logLevel, std::string message)
{
  apmon_utils::logger(logLevel, message.c_str());
  std::fflush(stdout);
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
