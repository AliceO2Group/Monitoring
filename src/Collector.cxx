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
  if (configFile->get<int>("InfoLoggerBackend/enable") == 1) {
    mBackends.emplace_back(std::make_unique<InfoLoggerBackend>());
  }
#ifdef _WITH_APPMON
  if (configFile->get<int>("AppMon/enable") == 1) {
    mBackends.emplace_back(std::make_unique<ApMonBackend>(
      configFile->get<string>("AppMon/pathToConfig").value()
    ));
  }
#endif

#ifdef _WITH_INFLUX
  if (configFile->get<int>("InfluxDB/enable") == 1) {
    std::string url = configFile->get<std::string>("InfluxDB/hostname").value() + ":" 
	            + configFile->get<std::string>("InfluxDB/port").value()
                    + "/write?db=" + configFile->get<std::string>("InfluxDB/db").value();
    mBackends.emplace_back(std::make_unique<InfluxBackend>(url));
  }
#endif

  if (configFile->get<int>("InfluxDBUDP/enable") == 1) {
    mBackends.emplace_back(std::make_unique<InfluxBackendUDP>(
      configFile->get<std::string>("InfluxDBUDP/hostname").value(), 
      configFile->get<int>("InfluxDBUDP/port").value()
    ));
  }
  setDefaultEntity();
  
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  if (configFile->get<int>("ProcessMonitor/enable") == 1) {
    int interval = configFile->get<int>("ProcessMonitor/interval").value();
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Collector::processMonitorLoop, this, interval);  
    MonInfoLogger::GetInstance() << "Process Monitor : Automatic updates enabled" << AliceO2::InfoLogger::InfoLogger::endm;
  }

  mDerivedHandler = std::make_unique<DerivedMetrics>(configFile->get<int>("DerivedMetrics/maxCacheSize").value());
}

Collector::~Collector()
{
  mMonitorRunning = false;
  if (mMonitorThread.joinable()) {
    mMonitorThread.join();
  }

}

void Collector::setDefaultEntity()
{
  char hostname[255];
  gethostname(hostname, 255);

  std::ostringstream format;
  format << hostname << "." << getpid();

  mEntity = format.str();
}
void Collector::setEntity(std::string entity) {
	mEntity = entity;
}

void Collector::sendProcessMonitorValues()
{
  ///                         type    name    value
  /// std::tuple<ProcessMonitorType, string, string>
  for (auto const pid : mProcessMonitor->getPidsDetails()) {
    switch (std::get<0>(pid)) {
      case ProcessMonitorType::INT : sendRawValue(boost::lexical_cast<int>(std::get<1>(pid)), std::get<2>(pid));
               break;
      case ProcessMonitorType::DOUBLE : sendRawValue(boost::lexical_cast<double>(std::get<1>(pid)), std::get<2>(pid));
               break;
      case ProcessMonitorType::STRING : sendRawValue(std::get<1>(pid), std::get<2>(pid));
               break;
    }
  }
}

void Collector::processMonitorLoop(int interval)
{
  while (mMonitorRunning) {
    sendProcessMonitorValues();
    std::this_thread::sleep_for (std::chrono::seconds(interval));
  }
}

void Collector::addMonitoredPid(int pid)
{
  mProcessMonitor->addPid(pid);
}

auto Collector::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

template<typename T>
void Collector::sendMetric(std::unique_ptr<Metric> metric, T)
{
  for (auto& b: mBackends) {
    b->send(boost::get<T>(metric->getValue()), metric->getName(), metric->getEntity(), metric->getTimestamp());
  }
}

void Collector::addDerivedMetric(std::string name, DerivedMetricMode mode) {
  mDerivedHandler->registerMetric(name, mode);
}

template<typename T> 
inline void Collector::sendRawValue(T value, std::string name, metric_time timestamp) const
{
  for (auto& b: mBackends) {
    b->send(value, name, mEntity, timestamp);
  }
}

template<typename T>
void Collector::send(T value, std::string name, metric_time timestamp)
{
  if (mDerivedHandler->isRegistered(name)) {
    try {
      std::unique_ptr<Metric> derived = mDerivedHandler->processMetric(value, name, mEntity, timestamp);
      if (derived != nullptr) sendMetric(std::move(derived), value);
    } 
    catch (boost::bad_get& e) {
      throw std::runtime_error("Derived metrics failed : metric " + name + " has incorrect type");
    }
  }
  sendRawValue(value, name, timestamp);
}

template void Collector::send(int, std::string, metric_time);
template void Collector::send(double, std::string, metric_time);
template void Collector::send(std::string, std::string, metric_time);
template void Collector::send(uint32_t, std::string, metric_time);

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
