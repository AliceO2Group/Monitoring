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
#include <utility>
#include <vector>

#include "MonLogger.h"
#include "ProcessDetails.h"
#include "Exceptions/MonitoringInternalException.h"

#include "Backends/InfoLoggerBackend.h"
#include "Backends/Flume.h"

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
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  mDerivedHandler = std::make_unique<DerivedMetrics>(1000);
}

void Collector::enableProcessMonitoring(int interval) {
#ifdef _OS_LINUX
  mMonitorRunning = true;
  mMonitorThread = std::thread(&Collector::processMonitorLoop, this, interval);
  MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
#else
  MonLogger::Get() << "!! Process Monitor : Automatic updates not supported" << MonLogger::End();
#endif
}

template<typename T>
Metric Collector::incrementMetric(T value, std::string name) {
  auto search = mIncrementCache.find(name);
  if (search != mIncrementCache.end()) {
    T current = boost::lexical_cast<T>(search->second.getValue());
    value += current;
    mIncrementCache.erase(search);
  }
  Metric result = Metric{value, name};
  mIncrementCache.insert(std::make_pair(name, result));
  return result;
}

template<typename T>
void Collector::increment(T value, std::string name) {
  send(incrementMetric(value, name));
}

void Collector::addBackend(std::unique_ptr<Backend> backend) {
   ProcessDetails processDetails{};
   backend->addGlobalTag("hostname", processDetails.getHostname());
   backend->addGlobalTag("name", processDetails.getProcessName());
   mBackends.push_back(std::move(backend));
}

Collector::~Collector()
{
  mMonitorRunning = false;
  if (mMonitorThread.joinable()) {
    mMonitorThread.join();
  }
}

void Collector::processMonitorLoop(int interval)
{
  // loopCount - no need to wait full sleep time to terminame the thread
  int loopCount = 0;
  while (mMonitorRunning) {
    std::this_thread::sleep_for (std::chrono::milliseconds(interval*10));
    if ((++loopCount % 100) != 0) continue;
    /// TODO!!! send all the merics as a single measurement
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
template void Collector::increment(int, std::string);
template void Collector::increment(double, std::string);
template void Collector::increment(uint64_t, std::string);
} // namespace Monitoring
} // namespace AliceO2
