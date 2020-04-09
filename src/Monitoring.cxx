// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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
  //mAutoPushInterval = 0;
  mMonitorRunning = false;
}

void Monitoring::enableBuffering(const std::size_t size)
{
  mBufferSize = size;
  mBuffering = true;
  for (std::underlying_type<Verbosity>::type i = 0; i < static_cast<std::underlying_type<Verbosity>::type>(Verbosity::Debug); i++) {
    mStorage[i].reserve(size);
  }
  MonLogger::Get() << "Buffering enabled (" << mStorage[0].capacity() << ")" << MonLogger::End();
}

void Monitoring::flushBuffer()
{
  if (!mBuffering) {
    MonLogger::Get() << "Cannot flush as buffering is disabled" << MonLogger::End();
    return;
  }
  for (auto& [verbosity, buffer] : mStorage) {
    for (auto& backend : mBackends) {
      if (matchVerbosity(backend->getVerbosity(), static_cast<Verbosity>(verbosity))) {
        backend->send(std::move(buffer));
        buffer.clear();
      }
    }
  }
}

void Monitoring::flushBuffer(const short index)
{
  for (auto& backend : mBackends) {
    if (matchVerbosity(backend->getVerbosity(), static_cast<Verbosity>(index))) {
      backend->send(std::move(mStorage[index]));
      mStorage[index].clear();
    }
  }
}

void Monitoring::enableProcessMonitoring(const unsigned int interval)
{
  mProcessMonitoringInterval = interval;
  if (!mMonitorRunning) {
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Monitoring::pushLoop, this);
  }
#ifdef O2_MONITORING_OS_LINUX
  MonLogger::Get() << "Process Monitor : Automatic updates enabled" << MonLogger::End();
#else
  MonLogger::Get() << "!! Process Monitor : Limited metrics available" << MonLogger::End();
#endif
}

void Monitoring::addGlobalTag(std::string_view key, std::string_view value)
{
  for (auto& backend : mBackends) {
    backend->addGlobalTag(key, value);
  }
}

void Monitoring::addGlobalTag(tags::Key key, tags::Value value)
{
  for (auto& backend : mBackends) {
    backend->addGlobalTag(
      tags::TAG_KEY[static_cast<std::underlying_type<tags::Key>::type>(key)],
      tags::TAG_VALUE[static_cast<std::underlying_type<tags::Value>::type>(value)]);
  }
}

void Monitoring::addBackend(std::unique_ptr<Backend> backend)
{
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
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  while (mMonitorRunning) {
    if (mProcessMonitoringInterval != 0 && (loopCount % (mProcessMonitoringInterval * 10)) == 0) {
      transmit(mProcessMonitor->getCpuAndContexts());
#ifdef O2_MONITORING_OS_LINUX
      transmit(mProcessMonitor->getMemoryUsage());
#endif
    }

    /*if (mAutoPushInterval != 0 && (loopCount % (mAutoPushInterval * 10)) == 0) {
      std::vector<Metric> metrics;
      for (auto metric : mPushStore) {
        metric.resetTimestamp();
        metrics.push_back(metric);
      }
      transmit(std::move(metrics));
    }*/
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    (loopCount >= 600) ? loopCount = 0 : loopCount++;
  }
}

/*ComplexMetric& Monitoring::getAutoPushMetric(std::string name, unsigned int interval)
{
  if (!mMonitorRunning) {
    mMonitorRunning = true;
    mMonitorThread = std::thread(&Monitoring::pushLoop, this);
    mAutoPushInterval = interval;
  }
  mPushStore.emplace_back(std::variant<int, std::string, double, uint64_t>{}, name);
  return mPushStore.back();
}*/

void Monitoring::transmit(std::vector<Metric>&& metrics)
{
  for (auto& backend : mBackends) {
    backend->send(std::move(metrics));
  }
}

void Monitoring::transmit(Metric&& metric)
{
  if (mBuffering) {
    auto index = static_cast<std::underlying_type<Verbosity>::type>(metric.getVerbosity());
    mStorage[index].push_back(std::move(metric));
    if (mStorage[index].size() >= mBufferSize) {
      flushBuffer(index);
    }
  } else {
    for (auto& backend : mBackends) {
      if (matchVerbosity(backend->getVerbosity(), metric.getVerbosity())) {
        backend->send(metric);
      }
    }
  }
}

void Monitoring::send(Metric&& metric, DerivedMetricMode mode)
{
  if (mode != DerivedMetricMode::NONE) {
    try {
      mDerivedHandler->process(metric, mode);
    } catch (MonitoringException& e) {
      MonLogger::Get() << e.what() << MonLogger::End();
    }
  }
  transmit(std::move(metric));
}

inline bool Monitoring::matchVerbosity(Verbosity backend, Verbosity metric)
{
  return (static_cast<std::underlying_type<Verbosity>::type>(backend) >= static_cast<std::underlying_type<Verbosity>::type>(metric));
}
} // namespace monitoring
} // namespace o2
