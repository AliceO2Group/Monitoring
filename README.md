# Monitoring
[![travis-ci](https://api.travis-ci.org/AliceO2Group/Monitoring.svg?branch=master)](https://travis-ci.org/AliceO2Group/Monitoring)
[![aliBuild](https://img.shields.io/badge/aliBuild-dashboard-lightgrey.svg)](https://alisw.cern.ch/dashboard/d/000000001/main-dashboard?orgId=1&var-storagename=All&var-reponame=All&var-checkname=build%2FMonitoring%2Fo2-dataflow%2F0&var-upthreshold=30m&var-minuptime=30)
[![codecov](https://codecov.io/gh/AliceO2Group/Monitoring/branch/dev/graph/badge.svg)](https://codecov.io/gh/AliceO2Group/Monitoring/branch/dev)
[![JIRA](https://img.shields.io/badge/JIRA-issues-blue.svg)](https://alice.its.cern.ch/jira/projects/OMON)
[![doxygen](https://img.shields.io/badge/doxygen-documentation-blue.svg)](https://aliceo2group.github.io/Monitoring/)

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
```bash
aliBuild build Monitoring --defaults o2-dataflow
```

  + Load the enviroment for Monitoring (in the `alice` directory)
```bash
alienv load Monitoring/latest
```

In case of an issue with `aliBuild` refer to [the official instructions](https://alice-doc.github.io/alice-analysis-tutorial/building).

### Manual
Manual installation of the O<sup>2</sup> Monitoring module.

#### Requirements
  + C++ compiler with C++17 support
  + Boost >= 1.56
  + libcurl
  + [ApMon](http://monalisa.caltech.edu/monalisa__Download__ApMon.html) (optional)

#### Monitoring module compilation

```bash
git clone https://github.com/AliceO2Group/Monitoring.git
cd Monitoring; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
```

## Getting started
### Monitoring instance
The recommended way of getting monitoring instance is `Get`ing it from  `MonitoringFactory` by passing backend's URI(s) as a parameter (comma separated if more than one).
The factory is accessible from `o2::monitoring` namespace.

```cpp
#include <MonitoringFactory.h>
using namespace o2::monitoring;
std::unique_ptr<Monitoring> monitoring = MonitoringFactory::Get("backend[-protocol]://host:port[/verbosity][?query]");
```

See table below to find out how to create `URI` for each backend:

| Backend name | Transport   | URI backend[-protocol] | URI query  | Default verbosity |
| ------------ |:-----------:|:----------------------:|:----------:| -----------------:|
| No-op        | -           | `no-op`                |            | -                 |
| InfluxDB     | UDP         | `influxdb-udp`         | -          | `info`            |
| InfluxDB     | Unix socket | `influxdb-unix`        | -          | `info`            |
| ApMon        | UDP         | `apmon`                | -          | `info`            |
| StdOut       | -           | `stdout`, `infologger` | [Prefix]   | `debug`           |
| Flume        | UDP         | `flume`                | -          | `info`            |

##### StdCout output format
```
[METRIC] <name>,<type> <value> <timestamp> <tags>
```

### Metrics
A metric consist of 5 parameters: name, value, timestamp, verbosity and tags.

| Parameter name | Type                             | Required | Default      |
| -------------- |:--------------------------------:|:--------:| ------------:|
| name           | string                           | yes      | -            |
| value          | int / double / string / uint64_t | yes      | -            |
| timestamp      | time_point&lt;system_clock&gt;   | no       | current time |
| verbosity      | Debug / Info / Prod              | no       | Info         |
| tags           | vector<unsigned int>             | no       | -            |

A metric can be constructed by providing required parameters (value and name):
```cpp
Metric{10, "name"}
```

#### Verbosity
There are 3 verbosity levels (the same as for backends): Debug, Info, Prod. The default verbosity is set using: `Metric::setDefaultVerbosity(verbosity)`.
To overwrite verbosity on per metric basis use third, optional parameter to metric constructor:
```cpp
Metric{10, "name", Verbosity::Prod}
```

Metrics need to match backends verbosity in order to be sent, eg. backend with `/info` verbosity will accept `Info` and `Prod` metrics only.

#### Tags
Each metric can be tagged with any number of [predefined tags](include/Monitoring/Tags.h).
In order to do so use `addTag(tags::Key, tags::Value)` or `addTag(tags::Key, unsigned short)` methods. The latter method allows assigning numeric value to a tag.

See the example: [examples/2-TaggedMetrics.cxx](examples/2-TaggedMetrics.cxx).

### Sending metric

```cpp
send(Metric&& metric, [DerivedMetricMode mode])
```

See how it works in the example: [examples/1-Basic.cxx](examples/1-Basic.cxx).

The `DerivedMetricMode` is optional and described in [Calculating derived metrics](#calculating-derived-metrics) section.

## Advanced features

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
monitoring->enableBuffering(const std::size_t maxSize)
...
monitoring->flushBuffer();
```

`enableBuffering` takes maximum buffer size as its parameter. The buffer gets full all values are flushed automatically.

See how it works in the example: [examples/10-Buffering.cxx](examples/10-Buffering.cxx).

### Calculating derived metrics
The module can calculate derived metrics. To do so, use optional `DerivedMetricMode mode` parameter of `send` method:
  + `DerivedMetricMode::NONE` - no action,
  + `DerivedMetricMode::RATE` - rate between two following metrics,
  + `DerivedMetricMode::AVERAGE` - average value of all metrics stored in cache.

Derived metrics are generated each time as new value is passed to the module. Their names are suffixed with derived mode name.

See how it works in the example: [examples/4-RateDerivedMetric.cxx](examples/4-RateDerivedMetric.cxx).

### Global tags
Glabal tags are tags that are added to each metric. The following tags are set to global by library itself:
  + `hostname`
  + `name` - process name

You can add your own global tag by calling `addGlobalTag(std::string_view key, std::string_view value)` or `addGlobalTag(tags::Key, tags::Value)`.

### Process monitoring
```cpp
enableProcessMonitoring([interval in seconds]);
```
The following metrics are generated every interval:
  + **cpuUsedPercentage** - percentage of a core usage over time interval
  + **involuntaryContextSwitches** - involuntary context switches over time interval
  + **memoryUsagePercentage** - ratio of the process's resident set size  to the physical memory on the machine, expressed as a percentage (Linux only)

### Automatic metric updates
Sometimes it's necessary to provide value every exact interval of time (even though value does not change). This can be done using `AutoPushMetric`.
```cpp
ComplexMetric& metric = monitoring->getAutoPushMetric("exampleMetric");
metric = 10;
```
See how it works in the example: [examples/11-AutoUpdate.cxx](examples/11-AutoUpdate.cxx).

### Regex verbosity policy
Overwrite metric verbosities using regex expression:
```
Metric::setVerbosityPolicy(Verbosity verbosity, const std::regex& regex)
```

## System monitoring, server-side backends installation and configuration
This guide explains manual installation. For `ansible` deployment see [AliceO2Group/system-configuration](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) gitlab repo.

+ [Collectd](doc/collectd.md)
+ [Flume](doc/flume.md)
+ [InfluxDB](doc/influxdb.md)
+ [Grafana](doc/grafana.md)
+ [MonALISA](http://monalisa.caltech.edu/monalisa__Documentation__Service_Installation_Guide.html) (external link)
