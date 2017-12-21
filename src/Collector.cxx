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

Collector::Collector() {
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

template <typename T>
void Collector::addBackend(const http::url& uri) {
   mBackends.emplace_back(std::make_unique<T>(uri));
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
template void Collector::addBackend<Backends::InfoLoggerBackend>(const http::url&);
#ifdef _WITH_APPMON
template void Collector::addBackend<Backends::ApMonBackend>(const http::url&);
#endif
template void Collector::addBackend<Backends::InfluxDB>(const http::url&);
template void Collector::addBackend<Backends::Flume>(const http::url&);
template void Collector::addBackend<Backends::Zabbix>(const http::url&);
} // namespace Monitoring
} // namespace AliceO2
