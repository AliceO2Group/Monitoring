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
  // loopCount - no need to wait full sleep time to terminame the thread
  int loopCount = 0;
  while (mMonitorRunning) {
    std::this_thread::sleep_for (std::chrono::milliseconds(interval*10));
    if ((++loopCount % 100) != 0) continue;
    for (auto&& metric : mProcessMonitor->getPidsDetails()) {
      send(std::move(metric));
    }
    loopCount = 0;
  }
}

void Collector::addDerivedMetric(std::string name, DerivedMetricMode mode) {
  mDerivedHandler->registerMetric(name, mode);
}

void Collector::send(Metric&& metric)
{
  for (auto& b: mBackends) {
    b->send(metric);
    try {
      b->send(mDerivedHandler->processMetric(metric));
    } catch (std::logic_error&) { }
  }
}

template<typename T>
void Collector::send(T value, std::string name)
{
  send({value, name});
}

template<typename T>
void Collector::sendTagged(T value, std::string name, std::vector<Tag>&& tags)
{
  Metric metric{value, name};
  metric.addTags(std::move(tags));
  send(std::move(metric));
}

template<typename T>
void Collector::sendTimed(T value, std::string name, std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  send({value, name, timestamp});
}

template void Collector::send(int, std::string);
template void Collector::send(double, std::string);
template void Collector::send(std::string, std::string);
template void Collector::send(uint32_t, std::string);

template void Collector::sendTagged(int, std::string, std::vector<Tag>&& tags);
template void Collector::sendTagged(double, std::string, std::vector<Tag>&& tags);
template void Collector::sendTagged(std::string, std::string, std::vector<Tag>&& tags);
template void Collector::sendTagged(uint32_t, std::string, std::vector<Tag>&& tags);


template void Collector::sendTimed(int, std::string, std::chrono::time_point<std::chrono::system_clock>& timestamp);
template void Collector::sendTimed(double, std::string, std::chrono::time_point<std::chrono::system_clock>& timestamp);
template void Collector::sendTimed(std::string, std::string, std::chrono::time_point<std::chrono::system_clock>& timestamp);
template void Collector::sendTimed(uint32_t, std::string, std::chrono::time_point<std::chrono::system_clock>& timestamp);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
