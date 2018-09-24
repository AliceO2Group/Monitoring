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

#include "Backends/InfoLoggerBackend.h"
#include "Backends/Flume.h"

#ifdef _WITH_APPMON
#include "Backends/ApMonBackend.h"
#endif
#include "Backends/InfluxDB.h"

namespace o2 
{
/// ALICE O2 Monitoring system
namespace monitoring 
{

Monitoring::Monitoring()
{
  mProcessMonitor = std::make_unique<ProcessMonitor>();
  mDerivedHandler = std::make_unique<DerivedMetrics>();
  mBuffering = false;
  mProcessMonitoringInterval = 0;
  mAutoPushInterval = 0;
  mMonitorRunning = false;
}

void Monitoring::enableBuffering(const unsigned int size)
{
  mBufferSize = size;
  mBuffering = true;
  mStorage.reserve(size);
  MonLogger::Get() << "Buffering enabled (" << mStorage.capacity() << ")" << MonLogger::End();
}

void Monitoring::flushBuffer() {
  if (!mBuffering) {
    MonLogger::Get() << "Cannot flush as buffering is disabled" << MonLogger::End();
    return;
  }
  send(std::move(mStorage));
  mStorage.clear();
}

void Monitoring::enableProcessMonitoring(const unsigned int interval) {
  mProcessMonitoringInterval = interval;
  if (!mMonitorRunning) {
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Monitoring::pushLoop, this);
  }
  #ifdef _OS_LINUX
  MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
  #else
  MonLogger::Get() << "!! Process Monitor : Limited metrics available" << MonLogger::End();
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

void Monitoring::addGlobalTag(std::string name, std::string value)
{
  for (auto& backend: mBackends) {
    backend->addGlobalTag(name, value);
  }
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
  if (mBuffering) {
    flushBuffer();
  }
}

void Monitoring::pushLoop()
{
  unsigned int loopCount = 0;
  std::this_thread::sleep_for (std::chrono::milliseconds(100));
  while (mMonitorRunning) {
    if (mProcessMonitoringInterval != 0 && (loopCount % (mProcessMonitoringInterval*10)) == 0) {
      send(mProcessMonitor->getCpuAndContexts());
      #ifdef _OS_LINUX
      send(mProcessMonitor->getMemoryUsage());
      #endif
    }

    if (mAutoPushInterval != 0 && (loopCount % (mAutoPushInterval*10)) == 0) {
      std::vector<Metric> metrics;
      for (auto& metric : mPushStore) {
        metrics.push_back(metric);
      }
      send(std::move(metrics));
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    (loopCount >= 600) ? loopCount = 0 : loopCount++;
  }
}

Metric& Monitoring::getAutoPushMetric(std::string name)
{
  if (mAutoPushInterval == 0) {
    MonLogger::Get() << "[WARN] AutoPush is not enabled" << MonLogger::End();
  }
  mPushStore.emplace_back(std::variant< int, std::string, double, uint64_t > {}, name);
  return mPushStore.back();
}

void Monitoring::sendGrouped(std::string measurement, std::vector<Metric>&& metrics)
{
  for (auto& b: mBackends) {
    b->sendMultiple(measurement, std::move(metrics));
  }
}

void Monitoring::send(std::vector<Metric>&& metrics)
{
  for (auto& b: mBackends) {
    b->send(std::move(metrics));
  }
}

void Monitoring::debug(Metric&& metric)
{
  for (auto& b: mBackends) {
    if (b->getVerbosity() == backend::Verbosity::DEBUG) {
      b->send(metric);
    }
  }
}

void Monitoring::enableAutoPush(unsigned int interval)
{
  if (!mMonitorRunning) {
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Monitoring::pushLoop, this);
  }
  mAutoPushInterval = interval;
}

void Monitoring::pushToBackends(Metric&& metric)
{
  if (mBuffering) {
    mStorage.push_back(std::move(metric));
    if (mStorage.size() >= mBufferSize) {
      flushBuffer();
    }
  } else {
    for (auto& b: mBackends) {
      b->send(metric);
    }
  }
}

void Monitoring::send(Metric&& metric, DerivedMetricMode mode)
{
  if (mode == DerivedMetricMode::RATE) {
    pushToBackends(mDerivedHandler->rate(metric));
  }

  if (mode == DerivedMetricMode::INCREMENT) {
    pushToBackends(mDerivedHandler->increment(metric));
  }

  pushToBackends(std::move(metric));
}

} // namespace monitoring
} // namespace o2
