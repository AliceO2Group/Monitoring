# Monitoring
[![travis-ci](https://api.travis-ci.org/AliceO2Group/Monitoring.svg?branch=master)](https://travis-ci.org/AliceO2Group/Monitoring)
[![aliBuild](https://img.shields.io/badge/aliBuild-dashboard-lightgrey.svg)](https://alisw.cern.ch/dashboard/d/000000001/main-dashboard?orgId=1&var-storagename=All&var-reponame=All&var-checkname=build%2FMonitoring%2Fo2-dataflow%2F0&var-upthreshold=30m&var-minuptime=30)
[![codecov](https://codecov.io/gh/AliceO2Group/Monitoring/branch/dev/graph/badge.svg)](https://codecov.io/gh/AliceO2Group/Monitoring/branch/dev)
[![JIRA](https://img.shields.io/badge/JIRA-issues-blue.svg)](https://alice.its.cern.ch/jira/projects/OMON)

Monitoring module allows to inject user defined metrics and monitor the process itself. It supports multiple backends, protocols and data formats.

## Table of contents
1. [Installation](#installation)
2. [Getting started](#getting-started)
3. [Features and additional information](#features-and-additional-information)
4. [Code snippets](#code-snippets)
5. [System monitoring and server-side backends installation and configuration](#system-monitoring-server-side-backends-installation-and-configuration)

## Installation

### aliBuild
<strong>[Click here if you don't have aliBuild installed](https://alice-doc.github.io/alice-analysis-tutorial/building/)</strong>
<br>

+ Compile `Monitoring` and its dependecies via `aliBuild`
~~~
aliBuild init Monitoring@master
aliBuild build Monitoring --defaults o2-daq
~~~

+ Load the enviroment for Monitoring (in the `alice` directory)
~~~
alienv load Monitoring/latest
~~~

In case of an issue with `aliBuild` refer to [the official instructions](https://alice-doc.github.io/alice-analysis-tutorial/building).

### Manual
Manual installation of the O<sup>2</sup> Monitoring module.

#### Requirements
+ C++ compiler with C++14 support, eg.:
  + `gcc-c++` package from `devtoolset-6` on CentOS 7
  + `clang++` on Mac OS
+ Boost >= 1.56
+ libcurl
+ [ApMon](http://monalisa.caltech.edu/monalisa__Download__ApMon.html) (optional)

#### Monitoring module compilation

```
git clone https://github.com/AliceO2Group/Monitoring.git
cd Monitoring; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
```

## Getting started
### Monitoring instance
The recommended way of getting (`unique_ptr` to) monitoring instance is `Get`ing it from  `MonitoringFactory` by passing backend URI(s) as a parameter (comma seperated if more than one).
The library is accessible from `o2::monitoring` namespace.

```cpp
#include <MonitoringFactory.h>
using namespace o2::monitoring;
std::unique_ptr<Monitoring> monitoring = MonitoringFactory::Get("backend[-protocol]://host:port[/verbosity][?query]");
```

See table below to find out how to create `URI` for each backend:

| Backend name | Transport | URI backend[-protocol] | URI query        | Default verbosity |
| ------------ |:---------:|:----------------------:|:----------------:| -----------------:|
| InfluxDB     | HTTP      | `influxdb-http`        | `/write?db=<db>` | `prod`            |
| InfluxDB     | UDP       | `influxdb-udp`         | -                | `prod`            |
| ApMon        | UDP       | `apmon`                | -                | `prod`            |
| Local InfoLogger | -     | `infologger://`        | -                | `debug`           |
| InfoLogger   | TCP       | `infologger`           | -                | `prod`            |
| Flume        | UDP       | `flume`                | -                | `prod`            |

Multiple backends may be used at the same time, URLs should be separated by `,` (comma).

### Sending metric

```cpp
send(Metric&& metric, [DerivedMetricMode mode])
```
Where metric constructor receives following parameters:
  - `T value`
  - `std::string& name`
  - `[time_point<system_clock> timestamp]`


The `DerivedMetricMode` is described in [Calculating derived metrics](#calculating-derived-metrics) section.

See how it works in the example: [examples/1-Basic.cxx](examples/1-Basic.cxx)

### Debug metrics
Debug metrics can be send by a similar method to above's `send`:
```cpp
debug(Metric&& metric)
```

The difference is that debug metrics are only passed to backends which verbosity level is set to `debug`.

Each backend has its default verbosity (see backend in [Monitoring instance](#monitoring-instance) section). This can be changed by defining path of a backend URL:
- `/prod` - only `send` metrics are passed to the backend
- `/debug` - all the metrics are passed to the backend


### Customized metrics
Two additional methods can be chained the to `send(Metric&& metric)` in order to __insert custom tags__ or __set custom timestamp__:
   + `addTags(std::vector<Tag>&& tags)`
   + `setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp)`

See how it works in the example: [examples/2-TaggedMetrics.cxx](examples/2-TaggedMetrics.cxx), [examples/3-UserDefinedTimestamp.cxx](examples/3-UserDefinedTimestamp.cxx).

## Features and additional information

### Sending more than one metric
In order to send more than one metric in a packet group them into vector:
```cpp
monitoring->send(std::vector<Metric>&& metrics);
```

It's also possible to send multiple, grouped values (only `Flume` and `InfluxDB` backends are supported); For example `cpu` metric can be composed of `cpuUser`, `cpuSystem` values.
```cpp
void sendGroupped(std::string name, std::vector<Metric>&& metrics)
```

See how it works in the example: [examples/8-Multiple.cxx](examples/8-Multiple.cxx)

### Buffering metrics
In order to avoid sending each metric separately, metrics can be temporary stored in the buffer and flushed at the most convenient moment.
This feature can be operated with following two methods:
```cpp
monitoring->enableBuffering(const unsigned int maxSize)
...
monitoring->flushBuffer();
```

`enableBuffering` takes maximum buffer size as its parameter. The buffer gets full all values are flushed automatically.

See how it works in the example: [examples/10-Buffering.cxx](examples/10-Buffering.cxx).

### Metrics
Metrics consist of 4 parameters: name, value, timestamp and tags.

| Parameter name | Type                                          | Required | Default          |
| -------------- |:---------------------------------------------:|:--------:| ----------------:|
| name           | string                                        | yes      | -                |
| value          | int / double / string / uint64_t    | yes      | -                |
| timestamp      | chrono::time_point&lt;std::chrono::system_clock&gt; | no       | current timestamp     |
| tags           | vector<Tag>                                   | no       | -**                |

**Default tag set is process specific and included in each metric:
+ hostname
+ PID
+ process name

### Calculating derived metrics
The module can calculate derived metrics. To do so, use optional `DerivedMetricMode mode` parameter of `send` method:
+ `DerivedMetricMode::NONE` - no action,
+ `DerivedMetricMode::RATE` - rate between two following metrics,
+ `DerivedMetricMode::AVERAGE` - average value of all metrics stored in cache.

Derived metrics are generated each time as new value is passed to the module. Their names are suffixed with derived mode name.

See how it works in the example: [examples/4-RateDerivedMetric.cxx](examples/4-RateDerivedMetric.cxx).

### Global tags
Glabal tags are tags that are added to each metric. The following tags are set to global by library itself:
- `hostname`
- `name` - process name

You can add your own global tag by calling `addGlobalTag(std::string name, std::string value)`.

### Monitoring process
```cpp
enableProcessMonitoring([interval in seconds]);
```
The following metrics are generated every interval:
+ **cpuUsedPercentage** - percentage of a core usage over time interval
+ **involuntaryContextSwitches** - involuntary context switches over time interval
+ **memoryUsagePercentage** - ratio of the process's resident set size  to the physical memory on the machine, expressed as a percentage (Linux only)

### Automatic metric updates
When global, higher level metrics are created it's necessary to provide values every interval of time (even though values does not change). This can be done using `AutoPushMetric`. The
```cpp
Metric& metric = monitoring->getAutoPushMetric("exampleMetric");
metric = 10;
```
See how it works in the example: [examples/11-AutoUpdate.cxx](examples/11-AutoUpdate.cxx).

## System monitoring, server-side backends installation and configuration
This guide explains manual installation. For `ansible` deployment see [AliceO2Group/system-configuration](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) gitlab repo.

 + [Collectd](doc/collectd.md)
 + [Flume](doc/flume.md)
 + [InfluxDB](doc/influxdb.md)
 + [Grafana](doc/grafana.md)
 + [MonALISA](http://monalisa.caltech.edu/monalisa__Documentation__Service_Installation_Guide.html) (external link)
