///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "MonInfoLogger.h"

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
  }
  else {
    MonInfoLogger::Get() << InfoLogger::Severity::Warning << "Reconfiguration of Monitoring forebidden! Valid configuration path: "
                         << MonitoringFactory::configPath  << InfoLogger::endm;
  }
}

Collector& MonitoringFactory::Get()
{
  if (MonitoringFactory::configPath.empty()) {
    MonInfoLogger::Get() << InfoLogger::Severity::Fatal << "Monitoring hasn't been configured"
                         << InfoLogger::endm;
    throw std::runtime_error("Monitoring hasn't been configured");
  }
  static Collector instance(MonitoringFactory::configPath);
  return instance;
}

std::unique_ptr<Collector> MonitoringFactory::Create(const std::string& configPath) {
  return std::make_unique<Collector>(configPath);
}

} // namespace Monitoring
} // namespace AliceO
