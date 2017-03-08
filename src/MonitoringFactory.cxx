///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "Exceptions/MonitoringException.h"
#include "MonInfoLogger.h"
#include <Configuration/ConfigurationFactory.h>

namespace AliceO2 
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

using AliceO2::InfoLogger::InfoLogger;

std::string MonitoringFactory::configPath = "";

void MonitoringFactory::Configure(const std::string& configPath)
{
  if (MonitoringFactory::configPath.empty()) {
    MonitoringFactory::configPath = configPath;
    Configuration::ConfigurationFactory::getConfiguration(configPath);
  }
  else {
    MonInfoLogger::Get() << InfoLogger::Severity::Warning << "Reconfiguration of Monitoring forebidden! Valid configuration path: "
                         << MonitoringFactory::configPath  << InfoLogger::endm;
  }
}

Collector& MonitoringFactory::Get()
{
  if (MonitoringFactory::configPath.empty()) {
    throw MonitoringException("MonitoringFactory", "Monitoring hasn't been configured");
  }
  static Collector instance(MonitoringFactory::configPath);
  return instance;
}

std::unique_ptr<Collector> MonitoringFactory::Create(const std::string& configPath) {
  return std::make_unique<Collector>(configPath);
}

} // namespace Monitoring
} // namespace AliceO
