///
/// \file Monitoring.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/Monitoring.h"

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

namespace o2 
{
/// ALICE O2 Monitoring system
namespace monitoring 
{

Monitoring::Monitoring() {
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  mDerivedHandler = std::make_unique<DerivedMetrics>();
}

void Monitoring::enableProcessMonitoring(int interval) {
#ifdef _OS_LINUX
  mMonitorRunning = true;
  mMonitorThread = std::thread(&Monitoring::processMonitorLoop, this, interval);
  MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
#else
  MonLogger::Get() << "!! Process Monitor : Automatic updates not supported" << MonLogger::End();
#endif
}

void Monitoring::startTimer(std::string name) {
  auto search = mTimers.find(name);
  if (search == mTimers.end()) {
    auto now = std::chrono::steady_clock::now();
    mTimers.insert(std::make_pair(name, now));
  } else {
    MonLogger::Get() << "Monitoring timer : Timer for " << name << " already started" << MonLogger::End();
  }
}

void Monitoring::stopAndSendTimer(std::string name) {
  auto search = mTimers.find(name);
  if (search != mTimers.end()) {
    auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    auto start = std::chrono::duration_cast <std::chrono::milliseconds>(search->second.time_since_epoch()).count();
    uint64_t duration = now - start;
    send({duration, name});
  } else {
    MonLogger::Get() << "Monitoring timer : Cannot stop " << name << " timer as it hasn't started" << MonLogger::End();
  }
}

template<typename T>
Metric Monitoring::incrementMetric(T value, std::string name) {
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
void Monitoring::increment(T value, std::string name) {
  send(incrementMetric(value, name));
}

void Monitoring::addBackend(std::unique_ptr<Backend> backend) {
   ProcessDetails processDetails{};
   backend->addGlobalTag("hostname", processDetails.getHostname());
   backend->addGlobalTag("name", processDetails.getProcessName());
   mBackends.push_back(std::move(backend));
}

Monitoring::~Monitoring()
{
  mMonitorRunning = false;
  if (mMonitorThread.joinable()) {
    mMonitorThread.join();
  }
}

void Monitoring::processMonitorLoop(int interval)
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

void Monitoring::send(std::string measurement, std::vector<Metric>&& metrics)
{
  for (auto& b: mBackends) {
    b->sendMultiple(measurement, std::move(metrics));
  }
}

void Monitoring::send(Metric&& metric, DerivedMetricMode mode)
{
  if (mode == DerivedMetricMode::RATE) {
    mDerivedHandler->calculateRate(metric);
  }

  if (mode == DerivedMetricMode::INCREMENT) {

  }

  for (auto& b: mBackends) {
    b->send(metric);
  }
}

template void Monitoring::increment(int, std::string);
template void Monitoring::increment(double, std::string);
template void Monitoring::increment(uint64_t, std::string);
} // namespace monitoring
} // namespace o2
