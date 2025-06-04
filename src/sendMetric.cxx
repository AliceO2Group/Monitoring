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

// simple command-line program used to send metrics using the monitoring lib

#include <Monitoring/MonitoringFactory.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
using namespace o2::monitoring;

void print_usage()
{
  printf("Monitoring command line utility to inject metrics.\n");
  printf("Usage: o2-monitoring-send [options]\n");
  printf("Options: \n");
  printf("  -u ...           Set monitoring URI (for metric output).\n");
  printf("  -m ...           Set metric name.\n");
  printf("  -i ...           Set metric value (int).\n");
  printf("  -v               Verbose mode.\n");
  printf("  -h               This help.\n");
  printf("\nExample: o2-monitoring-send -u influxdb-stdout:// -m test.metric -i 12345\n");
}


int main(int argc, char** argv)
{

  bool verbose = 0; // if set, prints detailed messages
  
  std::unique_ptr<Monitoring> monitoringCollector;
  char option;

  bool isOk = 1;
  const char *monitoringURI = nullptr;
  const char *monitoringValue = nullptr;
  const char *monitoringMetric = nullptr;

  // read options
  while ((option = getopt(argc, argv, "hvu:i:m:")) != -1) {
    switch (option) {
 
      case 'u': {
        monitoringURI = optarg;
      } break;
      
      case 'i': {
        monitoringValue = optarg;
      } break;
      
      case 'm': {
        monitoringMetric = optarg;
      } break;
     
      case 'v': {
        verbose = 1;
      } break;

      case 'h':
        print_usage();
        return 0;

      default:
        print_usage();
        return -1;
    }
  }

  if (monitoringURI == nullptr) {
    printf("Unspecified monitoring URI.\n");
    isOk = 0;
  }

  if (monitoringMetric == nullptr) {
    printf("Unspecified monitoring metric.\n");
    isOk = 0;
  }

  if (monitoringValue == nullptr) {
    printf("Unspecified monitoring value.\n");
    isOk = 0;
  }
  
  if (!isOk) {
    printf("Failed to send metric: bad parameters.\n\n\n");
    print_usage();
    return -1;
  }

  // conversions
  int monitoringValueI = atoi(monitoringValue);

  // disable logs from monitoring lib	
  setenv("O2_INFOLOGGER_MODE", "none", 1);

  if (verbose) {
    // summarize status
    printf("URI = %s\n", monitoringURI);
    printf("Metric = %s\n", monitoringMetric);
    printf("Value = %d (int)\n", monitoringValueI);
    printf("\n");
  }

  isOk = 0;
  try {
    monitoringCollector = MonitoringFactory::Get(monitoringURI);
    monitoringCollector->send({ monitoringValueI, monitoringMetric });
    isOk = 1;
  }
  catch (const std::exception &exc) {
    printf("Exception: %s\n", exc.what());
  }
  catch (...) {
    printf("Undefined exception\n");
  }
  
  if (!isOk) {
    printf("Failed to send metric\n");
    return -1;
  }

  if (verbose) {
    printf("\nSuccess\n");
  }
  return 0;
}

