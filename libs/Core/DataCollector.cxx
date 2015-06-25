/// \file    DataCollector.cxx
/// \brief   Implementation of the DataCollector class.
///
/// \author  Vasco Barroso, CERN

#include <Core/DataCollector.h>
#include <Core/FileNotFoundException.h>

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <stdio.h>

namespace AliceO2 {
namespace Monitoring {
namespace Core {

DataCollector::DataCollector(const std::string configurationFile, const std::string defaultCluster)
    : mConfigurationFile(configurationFile),
      mDefaultCluster(defaultCluster)
{
  try {
    // create ApMon object
    mApMon = new ApMon(const_cast<char *>(configurationFile.c_str()));

    // configure process monitoring
    configureProcessMonitoring();

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

const string& DataCollector::getConfigurationFile() const
{
  return mConfigurationFile;
}

const string& DataCollector::getDefaultCluster() const
{
  return mDefaultCluster;
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

std::string DataCollector::getHostname()
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

std::string DataCollector::getProcessUniqueId()
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

void DataCollector::configureProcessMonitoring()
{
  // get current working dir
  char *currentWorkingDir;
  currentWorkingDir = get_current_dir_name();

  log(INFO, "Setting default Cluster to '" + getDefaultCluster() + "'");
  log(INFO, "Setting process unique ID to '" + getProcessUniqueId() + "'");
  log(INFO, "Setting working dir to '" + std::string(currentWorkingDir) + "'");

  // add process monitoring
  mApMon->addJobToMonitor(getpid(), currentWorkingDir, const_cast<char *>(getDefaultCluster().c_str()), const_cast<char *>(getProcessUniqueId().c_str()));

  free(currentWorkingDir);

}

void DataCollector::log(int logLevel, std::string message)
{
  apmon_utils::logger(logLevel, message.c_str());
  fflush(stdout);
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
