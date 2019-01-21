///
/// \file Monitoring.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/Monitoring.h"
#include "Exceptions/MonitoringException.h"

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "MonLogger.h"
#include "ProcessDetails.h"

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

void Monitoring::enableBuffering(const std::size_t size)
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

void Monitoring::addGlobalTag(std::string_view name, std::string_view value)
{
  for (auto& backend: mBackends) {
    backend->addGlobalTag(name, value);
  }
}

void Monitoring::addGlobalTag(const unsigned int tag)
{
  for (auto& backend: mBackends) {
    backend->addGlobalTag(tags::TAG_ARRAY[tag].first, tags::TAG_ARRAY[tag].second);
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
      for (auto metric : mPushStore) {
        metric.resetTimestamp();
        metrics.push_back(metric);
      }
      send(std::move(metrics));
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    (loopCount >= 600) ? loopCount = 0 : loopCount++;
  }
}

ComplexMetric& Monitoring::getAutoPushMetric(std::string name, unsigned int interval)
{
  if (!mMonitorRunning) {
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Monitoring::pushLoop, this);
    mAutoPushInterval = interval;
  }
  mPushStore.emplace_back(boost::variant< int, std::string, double, uint64_t > {}, name);
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

void Monitoring::transmit(Metric&& metric)
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
  if (mode != DerivedMetricMode::NONE) {
     try { transmit(mDerivedHandler->process(metric, mode)); }
     catch (MonitoringException& e) { MonLogger::Get() << e.what() << MonLogger::End(); }
  }
  transmit(std::move(metric));
}

} // namespace monitoring
} // namespace o2
