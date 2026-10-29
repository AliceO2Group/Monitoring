// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file 17-ProcessMonitorMetrics.cxx
/// \brief Shows the metrics published by process monitoring, while the process allocates and frees memory
///

#include "Monitoring/MonitoringFactory.h"
#include "Monitoring/ProcessMonitor.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>

using namespace o2::monitoring;

int main()
{
  printf("Monitoring test - pid %d\n", getpid());

  printf("\nMetrics defined for each measurement (* = optional, may be missing on some systems):\n");
  for (auto measurement : {PmMeasurement::Cpu, PmMeasurement::Mem, PmMeasurement::Smaps}) {
    for (const auto& metric : ProcessMonitor::getMetricsInfo(measurement)) {
      printf("  %-5s %s%s\n",
             measurement == PmMeasurement::Cpu ? "Cpu" : measurement == PmMeasurement::Mem ? "Mem" : "Smaps",
             metric.name.c_str(), metric.optional ? " *" : "");
    }
  }

  printf("\nCreating monitoring object\n");
  auto monitoring = MonitoringFactory::Get("influxdb-stdout://");

  // metrics published every 2 seconds, from now
  monitoring->enableProcessMonitoring(2, {PmMeasurement::Cpu, PmMeasurement::Mem, PmMeasurement::Smaps});

  constexpr size_t allocSize = 1024LL * 1024LL * 1024LL; // 1 GB
  char* ptr = nullptr;

  for (int i = 0; i < 15; i++) {
    printf("\n#%02d\n", i);

    if (i == 2) {
      printf("malloc %zu bytes\n", allocSize);
      ptr = static_cast<char*>(malloc(allocSize));
      if (ptr == nullptr) {
        printf("malloc failed\n");
      } else {
        // touch the pages, so that they are actually resident
        memset(ptr, 1, allocSize);
      }
    }

    if (i == 10) {
      printf("free\n");
      free(ptr);
      ptr = nullptr;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // publishes the final metrics (averages, totals) and stops process monitoring
  printf("\nDone\n");
  monitoring->finalizeProcessMonitoring();

  return 0;
}
