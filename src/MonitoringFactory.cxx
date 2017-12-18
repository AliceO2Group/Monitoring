///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "Exceptions/MonitoringException.h"
#include "MonLogger.h"

namespace AliceO2 
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

std::string MonitoringFactory::configPath = "";

void MonitoringFactory::Configure(const std::string& configPath)
{
  if (MonitoringFactory::configPath.empty()) {
    //MonitoringFactory::configPath = configPath;
    //Configuration::ConfigurationFactory::getConfiguration(configPath);
  }
  else {
    MonLogger::Get() << "Reconfiguration of Monitoring forebidden! Valid configuration path: "
      << MonitoringFactory::configPath  << MonLogger::End();
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
