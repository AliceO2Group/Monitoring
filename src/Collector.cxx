#include "Monitoring/Collector.h"

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "MonInfoLogger.h"
#include "InfoLoggerBackend.h"

#ifdef _WITH_APPMON
#include "ApMonBackend.h"
#endif

#ifdef _WITH_INFLUX
#include "InfluxBackend.h"
#endif

namespace AliceO2 {
namespace Monitoring {
namespace Core {

Collector::Collector(ConfigFile &configFile) {
  if (configFile.getValue<int>("InfoLoggerBackend.enable") == 1)
    mBackends.emplace_back(std::unique_ptr<Backend>(new InfoLoggerBackend()));

#ifdef _WITH_APPMON
  if (configFile.getValue<int>("AppMon.enable") == 1)
    mBackends.emplace_back(std::unique_ptr<Backend>(
      new ApMonBackend(configFile.getValue<string>("AppMon.pathToConfig"))
    ));
#endif

#ifdef _WITH_INFLUX
  if (configFile.getValue<int>("InfluxDB.enable") == 1)
    mBackends.emplace_back(std::unique_ptr<Backend>(
      new InfluxBackend(configFile.getValue<string>("InfluxDB.writeUrl"))
    ));
#endif
  setUniqueEntity();
  
  mProcessMonitor = std::unique_ptr<ProcessMonitor>(new ProcessMonitor());
  if (configFile.getValue<int>("ProcessMonitor.enable") == 1) {
    int interval = configFile.getValue<int>("ProcessMonitor.interval");
    mMonitorRunning = true;
    monitorThread = std::thread(&Collector::processMonitorLoop, this, interval);  
    MonInfoLogger::GetInstance() << "Process Monitor : Automatic updates enabled" << AliceO2::InfoLogger::InfoLogger::endm;
  }
}

Collector::~Collector() {
  mMonitorRunning = false;
  if (monitorThread.joinable()) {
    monitorThread.join();
  }

}

void Collector::setUniqueEntity() {
  char hostname[255];
  gethostname(hostname, 255);

  std::ostringstream format;
  format << hostname << "." << getpid();

  mUniqueEntity = format.str();
}
void Collector::setEntity(std::string entity) {
	mUniqueEntity = entity;
}

void Collector::monitorUpdate() {
  ///           1-3    name    value
  /// std::tuple<int, string, string>
  for (auto const pid : mProcessMonitor->getPidsDetails()) {
    switch (std::get<0>(pid)) {
      // convert to int
      case 0 : sendDirect( std::stoi(std::get<1>(pid)), std::get<2>(pid));
               break;
      // convert to double
      case 1 : sendDirect( std::stod(std::get<1>(pid)), std::get<2>(pid));
               break;
      // do not convert (string)
      case 2 : sendDirect(std::get<1>(pid), std::get<2>(pid));
               break;
    }
  }
}

void Collector::processMonitorLoop(int interval) {
  while (mMonitorRunning) {
    monitorUpdate();
    std::this_thread::sleep_for (std::chrono::seconds(interval));
  }
}

void Collector::addMonitoredPid(int pid) {
  mProcessMonitor->addPid(pid);
}

std::chrono::time_point<std::chrono::system_clock> Collector::getCurrentTimestamp() {
  return std::chrono::system_clock::now();
}

template<typename T>
void Collector::sendMetric(std::unique_ptr<Metric> &&metric, T) {
  for (auto& b: mBackends) {
    b->send(boost::get<T>(metric->getValue()), metric->getName(), metric->getEntity(), metric->getTimestamp());
  }
}

void Collector::addDerivedMetric(DerivedMetricMode mode, std::string name) {
  mDerivedHandler.registerMetric(mode, name);
}

template<typename T> 
inline void Collector::sendDirect(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp) const {
  for (auto& b: mBackends) {
    b->send(value, name, mUniqueEntity, timestamp);
  }
}

template<typename T>
void Collector::send(T value, std::string name, std::chrono::time_point<std::chrono::system_clock> timestamp) {
  if (mDerivedHandler.isRegistered(name)) {
    try {
      std::unique_ptr<Metric> derived = mDerivedHandler.processMetric(value, name, mUniqueEntity, timestamp);
      if (derived != nullptr) sendMetric(std::move(derived), value);
    } catch(boost::bad_get e) {
      throw std::runtime_error("Derived metrics failed : metric " + name + " has incorrect type");
    }
  }
  sendDirect(value, name, timestamp);
}

template void Collector::send(int, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(double, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(std::string, std::string, std::chrono::time_point<std::chrono::system_clock>);
template void Collector::send(uint32_t, std::string, std::chrono::time_point<std::chrono::system_clock>);

template void Collector::sendDirect(int, std::string, std::chrono::time_point<std::chrono::system_clock>) const;
template void Collector::sendDirect(double, std::string, std::chrono::time_point<std::chrono::system_clock>) const;
template void Collector::sendDirect(std::string, std::string, std::chrono::time_point<std::chrono::system_clock>) const;
template void Collector::sendDirect(uint32_t, std::string, std::chrono::time_point<std::chrono::system_clock>) const;

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

