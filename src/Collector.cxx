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

#include "MonLogger.h"
#include "ProcessDetails.h"
#include "Exceptions/MonitoringInternalException.h"
#include "Backends/InfoLoggerBackend.h"
#include "Backends/Flume.h"
#include "Backends/Zabbix.h"

#ifdef _WITH_APPMON
#include "Backends/ApMonBackend.h"
#endif

#ifdef _WITH_INFLUX
#include "Backends/InfluxDB.h"
#endif

namespace AliceO2 
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

Collector::Collector(const std::string& configPath)
{
/*
  MonLogger::Get() << "Creating Monitoring instance from configuration: "
    << configPath << MonLogger::End();

  std::unique_ptr<Configuration::ConfigurationInterface> configFile =
		  Configuration::ConfigurationFactory::getConfiguration(configPath);
  if (configFile->get<int>("InfoLoggerBackend/enable").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::InfoLoggerBackend>());
  }
  else {
    MonLogger::Get() << "InfoLogger backend disabled" << MonLogger::End();
  }
#ifdef _WITH_APPMON
  if (configFile->get<int>("ApMon/enable").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::ApMonBackend>(
      configFile->get<string>("ApMon/pathToConfig").value()
    ));
  }
  else {
    MonLogger::Get() << "ApMon backend disabled" << MonLogger::End();
  }
#endif

#ifdef _WITH_INFLUX
  if (configFile->get<int>("InfluxDB/enableUDP").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::InfluxDB>(
      configFile->get<std::string>("InfluxDB/hostname").value(), 
      configFile->get<int>("InfluxDB/port").value()
    ));
 } 
 else {
  MonLogger::Get() << "InfluxDB/UDP backend disabled" << MonLogger::End();
 }
 if (configFile->get<int>("InfluxDB/enableHTTP").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::InfluxDB>(
      configFile->get<std::string>("InfluxDB/hostname").value(),
      configFile->get<int>("InfluxDB/port").value(),
      configFile->get<std::string>("InfluxDB/db").value()
    ));
  }
  else {
    MonLogger::Get() << "InfluxDB/HTTP backend disabled" << MonLogger::End();
  }
#endif
  
  if (configFile->get<int>("Flume/enable").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::Flume>(
      configFile->get<std::string>("Flume/hostname").value(),
      configFile->get<int>("Flume/port").value()
    )); 
  }
  else {
    MonLogger::Get() << "Flume backend disabled" << MonLogger::End();
  }

  if (configFile->get<int>("Zabbix/enable").value_or(0) == 1) {
    mBackends.emplace_back(std::make_unique<Backends::Zabbix>(
      configFile->get<std::string>("Zabbix/hostname").value(),
      configFile->get<int>("Zabbix/port").value()
    )); 
  }
  else {
    MonLogger::Get() << "Zabbix backend disabled" << MonLogger::End();
  }

#ifdef _OS_LINUX  
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  if (configFile->get<int>("ProcessMonitor/enable").value_or(0) == 1) {
    int interval = configFile->get<int>("ProcessMonitor/interval").value();
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Collector::processMonitorLoop, this, interval);  
    MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
  }
#endif

  mDerivedHandler = std::make_unique<DerivedMetrics>(configFile->get<int>("DerivedMetrics/maxCacheSize").value_or(0));
  setDefaultTags();
*/
}

Collector::Collector() {
  mBackends.emplace_back(std::make_unique<Backends::InfoLoggerBackend>());

#ifdef _OS_LINUX
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  int interval = 5;
  mMonitorRunning = true;
  mMonitorThread = std::thread(&Collector::processMonitorLoop, this, interval);
  MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
#endif

  mDerivedHandler = std::make_unique<DerivedMetrics>(1000);
  setDefaultTags();
}

template<typename S>
void Collector::addBackend(std::string url) {
  std::cout << url << std::endl;
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
    // send pmem, pcpu, etime
    for (auto&& metric : mProcessMonitor->getPidStatus()) {
      send(std::move(metric));
    }
    // send bytesReceived and bytedTransmitted per interface
    for (auto&& metric : mProcessMonitor->getNetworkUsage()) {
      send(std::move(metric));
    }   
    loopCount = 0;
  }
}

void Collector::addDerivedMetric(std::string name, DerivedMetricMode mode) {
  mDerivedHandler->registerMetric(name, mode);
}

void Collector::send(std::string measurement, std::vector<Metric>&& metrics)
{
  for (auto& b: mBackends) {
    b->sendMultiple(measurement, std::move(metrics));
  }
}

void Collector::send(Metric&& metric)
{
  for (auto& b: mBackends) {
    b->send(metric);
  }
  if (mDerivedHandler->isRegistered(metric.getName())) {
    try {
      Metric&& derived = mDerivedHandler->processMetric(metric);
      for (auto& b: mBackends) {
        b->send(derived);
      }   
    } catch (MonitoringInternalException&) { }
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
template void Collector::send(uint64_t, std::string);

template void Collector::addBackend<Backends::InfluxDB>(std::string);
template void Collector::addBackend<Backends::Flume>(std::string);
template void Collector::addBackend<Backends::InfoLoggerBackend>(std::string);
template void Collector::addBackend<Backends::Zabbix>(std::string);
#ifdef _WITH_APPMON
template void Collector::addBackend<Backends::ApMonBackend>(std::string);
#endif
} // namespace Monitoring
} // namespace AliceO2
