///
/// \file Collector.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/Collector.h"

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <Configuration/ConfigurationFactory.h>

#include "MonInfoLogger.h"
#include "InfoLoggerBackend.h"
#include "InfluxBackendUDP.h"
#include "ProcessDetails.h"

#ifdef _WITH_APPMON
#include "ApMonBackend.h"
#endif

#ifdef _WITH_INFLUX
#include "InfluxBackend.h"
#endif

namespace AliceO2 
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

Collector::Collector(const std::string& configPath)
{
  std::unique_ptr<Configuration::ConfigurationInterface> configFile =
		  Configuration::ConfigurationFactory::getConfiguration(configPath);
  std::cout << configPath << std::endl;
  if (configFile->get<int>("InfoLoggerBackend.enable") == 1) {
    mBackends.emplace_back(std::make_unique<InfoLoggerBackend>());
  }
#ifdef _WITH_APPMON
  if (configFile->get<int>("AppMon.enable") == 1) {
    mBackends.emplace_back(std::make_unique<ApMonBackend>(
      configFile->get<string>("AppMon.pathToConfig").value()
    ));
  }
#endif

#ifdef _WITH_INFLUX
  if (configFile->get<int>("InfluxDB.enable") == 1) {
    std::string url = configFile->get<std::string>("InfluxDB.hostname").value() + ":" 
	            + configFile->get<std::string>("InfluxDB.port").value()
                    + "/write?db=" + configFile->get<std::string>("InfluxDB.db").value();
    mBackends.emplace_back(std::make_unique<InfluxBackend>(url));
  }
#endif

  if (configFile->get<int>("InfluxDBUDP.enable") == 1) {
    mBackends.emplace_back(std::make_unique<InfluxBackendUDP>(
      configFile->get<std::string>("InfluxDBUDP.hostname").value(), 
      configFile->get<int>("InfluxDBUDP.port").value()
    ));
  }
  
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  if (configFile->get<int>("ProcessMonitor.enable") == 1) {
    int interval = configFile->get<int>("ProcessMonitor.interval").value();
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Collector::processMonitorLoop, this, interval);  
    MonInfoLogger::Info() << "Process Monitor : Automatic updates enabled" << AliceO2::InfoLogger::InfoLogger::endm;
  }

  mDerivedHandler = std::make_unique<DerivedMetrics>(configFile->get<int>("DerivedMetrics.maxCacheSize").value());

  setDefaultTags();
}

Collector::~Collector()
{
  mMonitorRunning = false;
  if (mMonitorThread.joinable()) {
    mMonitorThread.join();
  }

}

void Collector::setDefaultTags()
{
  ProcessDetails details{};
  for (auto& b: mBackends) {
    b->addGlobalTag("pid", std::to_string(details.getPid()));
    b->addGlobalTag("hostname", details.getHostname());
    b->addGlobalTag("name", details.getProcessName());
  }
}

void Collector::processMonitorLoop(int interval)
{
  while (mMonitorRunning) {
    for (auto&& metric : mProcessMonitor->getPidsDetails()) {
      send(std::move(metric));
    }
    std::this_thread::sleep_for (std::chrono::seconds(interval));
  }
}

void Collector::addDerivedMetric(std::string name, DerivedMetricMode mode) {
  mDerivedHandler->registerMetric(name, mode);
}

void Collector::send(Metric&& metric)
{
  for (auto& b: mBackends) {
    b->send(metric);
    //std::unique_ptr<Metric> derived = mDerivedHandler->processMetric(std::move(metric));
    //b->send(derived.get());
  }
}

template<typename T>
void Collector::send(T value, std::string name)
{
  send({value, name});
}

template void Collector::send(int, std::string);
template void Collector::send(double, std::string);
template void Collector::send(std::string, std::string);
template void Collector::send(uint32_t, std::string);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
