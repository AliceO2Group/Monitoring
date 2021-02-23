# Monitoring
[![travis-ci](https://travis-ci.com/AliceO2Group/Monitoring.svg?branch=dev)](https://travis-ci.com/AliceO2Group/Monitoring)
[![aliBuild](https://img.shields.io/badge/aliBuild-dashboard-lightgrey.svg)](https://alisw.cern.ch/dashboard/d/000000001/main-dashboard?orgId=1&var-storagename=All&var-reponame=All&var-checkname=build%2FMonitoring%2Fo2-dataflow%2F0&var-upthreshold=30m&var-minuptime=30)
[![codecov](https://codecov.io/gh/AliceO2Group/Monitoring/branch/master/graph/badge.svg)](https://codecov.io/gh/AliceO2Group/Monitoring)
[![JIRA](https://img.shields.io/badge/JIRA-issues-blue.svg)](https://alice.its.cern.ch/jira/projects/OMON)
[![doxygen](https://img.shields.io/badge/doxygen-documentation-blue.svg)](https://aliceo2group.github.io/Monitoring/)

Monitoring module injects user custom metrics and monitors the process. It supports multiple backends, protocols and data formats.

## Table of contents
1. [Installation](#installation)
2. [Getting started](#getting-started)
3. [Advanced features](#advanced-features)
4. [System monitoring and server-side backends installation and configuration](#system-monitoring-server-side-backends-installation-and-configuration)

## Installation

<strong>[Click here if you don't have aliBuild installed](https://alice-doc.github.io/alice-analysis-tutorial/building/)</strong>
<br>

  + Compile `Monitoring` and its dependencies via `aliBuild`
```bash
aliBuild build Monitoring --defaults o2-dataflow
```

  + Load the environment for Monitoring (in the `alice` directory)
```bash
alienv load Monitoring/latest
```

## Getting started
### Monitoring instance
`Get` an instance from  `MonitoringFactory` by passing backend's URI(s) as a parameter (comma separated if more than one).
The factory is accessible from `o2::monitoring` namespace.

```cpp
#include <MonitoringFactory.h>
using namespace o2::monitoring;
std::unique_ptr<Monitoring> monitoring = MonitoringFactory::Get("backend[-protocol]://host:port[/verbosity][?query]");
```

See the table below to find `URI`s for supported backends:

| Backend name | Transport   | URI backend[-protocol] | URI query   | Default verbosity |
| ------------ |:-----------:|:----------------------:|:-----------:| -----------------:|
| No-op        | -           | `no-op`                | -           | -                 |
| InfluxDB     | UDP         | `influxdb-udp`         | -           | `info`            |
| InfluxDB     | Unix socket | `influxdb-unix`        | -           | `info`            |
| InfluxDB     | StdOut      | `influxdb-stdout`      | -           | `info`            |
| InfluxDB     | Kafka       | `influxdb-kafka`       | Kafka topic | `info`            |
| InfluxDB 2.x | HTTP        | `influxdbv2`           | `org=ORG&bucket=BUCKET&token=TOKEN` | `info` |
| ApMon        | UDP         | `apmon`                | -           | `info`            |
| StdOut       | -           | `stdout`, `infologger` | [Prefix]    | `debug`           |

### Metrics
A metric consist of 5 parameters:
- name - metric name
- values - vector of value and value name pairs
- timestamp - time of creation
- verbosity - metric "severity"
- tags - metric metadata represented as map

| Parameter name | Type                             | Required | Default                 |
| -------------- |:--------------------------------:|:--------:| -----------------------:|
| name           | string                           | yes      | -                       |
| values         | map&lt;string, int/double/string/uint64_t&gt; | no/1      | -                       |
| timestamp      | time_point&lt;system_clock&gt;   | no       | current time            |
| verbosity      | Enum (Debug/Info/Prod)           | no       | Verbosity::Info         |
| tags           | map             | no       | host and process names  |

A metric can be constructed by providing required parameters (value and metric name, value name is set to `value`):
```cpp
Metric{10, "name"}
```
#### Values
By default metric can be created with zero or one value (in such case value name is set to `value`). Any additional value may be added using `.addValue` method, therefore the following two metrics are identical:
```cpp
Metric{10, "name"}
Metric{"name"}.addValue(10, "value")
```

#### Tags
Each metric can be tagged with any number of [predefined tags](include/Monitoring/Tags.h).
In order to do so use `addTag(tags::Key, tags::Value)` or `addTag(tags::Key, unsigned short)` methods. The latter method allows assigning numeric value to a tag.

```cpp
Metric{10, "name"}.addTag(tags::Key::Subsystem, tags::Value::QC)
```

See the example: [examples/2-TaggedMetrics.cxx](examples/2-TaggedMetrics.cxx).

### Sending metric
Pass metric object to `send` method as l-value reference:
```cpp
send({10, "name"})
send(Metric{20, "name"}.addTag(tags::Key::CRU, 123))
send(Metric{"throughput"}.addValue(100, "tx").addValue(200, "rx"))
```

See how it works in the example: [examples/1-Basic.cxx](examples/1-Basic.cxx).

## Advanced features

### Metric verbosity
There are 3 verbosity levels (the same as for backends): Debug, Info, Prod. By default it is set to `Verbosity::Info`. The default value can be overwritten using: `Metric::setDefaultVerbosity(verbosity)`.
To overwrite verbosity on per metric basis use third, optional parameter to metric constructor:
```cpp
Metric{10, "name", Verbosity::Prod}
```

Metrics need to match backends verbosity in order to be sent, eg. backend with `/info` verbosity will accept `Info` and `Prod` metrics only.

### Buffering metrics
In order to avoid sending each metric separately, metrics can be temporary stored in the buffer and flushed at the most convenient moment.
This feature can be controlled with following two methods:
```cpp
monitoring->enableBuffering(const std::size_t maxSize)
...
monitoring->flushBuffer();
```

`enableBuffering` takes maximum buffer size as its parameter. The buffer gets full all values are flushed automatically.

See how it works in the example: [examples/10-Buffering.cxx](examples/10-Buffering.cxx).

### Calculating derived values
This feature can calculate derived values. To do so, use optional `DerivedMetricMode mode` parameter of `send` method:
```
send(Metric&& metric, [DerivedMetricMode mode])
```

Two modes are available:
  + `DerivedMetricMode::RATE` - rate between two following values,
  + `DerivedMetricMode::INCREMENT` - sum of all passed values.

The derived value is generated only from the first value of the metric and it is added to the same metric with the value name suffixed with `_rate`, `_increment` accordingly.

See how it works in the example: [examples/4-RateDerivedMetric.cxx](examples/4-RateDerivedMetric.cxx).

### Global tags
Global tags are added to each metric sent using given monitoring instance. `hostname` is set as global by default.

You can add your own global tag by calling `addGlobalTag(std::string_view key, std::string_view value)` or `addGlobalTag(tags::Key, tags::Value)`.

### Process monitoring

This feature provides basic performance status of the process. Note that is runs in separate thread (without mutex).

```cpp
enableProcessMonitoring([interval in seconds]);
```

As an alternative, without a need of a new thread, user can manually trigger:
```
pushProcessMonitoringMetrics()
```

Following metrics are generated every time interval:
CPU measurements:
 + **cpuUsedPercentage** - percentage of a core usage (kernel + user mode) over time interval
 + **involuntaryContextSwitches** - involuntary context switches over time interval
 + **cpuUsedAbsolute** - amount of time spent on process execution (in user and kernel mode) over time interval (expressed in microseconds)

Memory measurements: (Linux only)
 + **memoryUsagePercentage** - ratio of the process's virtual memory to memory available on the machine
 + **virtualMemorySize** - virtual memory reserved by process (expressed in kB)
 + **residentSetSize** - resident set size reserved by process (expressed in kB)

Additional metrics are generated at the end of process execution:
CPU measurements:
 + **cpuTimeConsumedByProcess** - total amount of time spent on process execution (in user and kernel mode) (expressed in microseconds)
 + **averageCpuUsedPercentage** - average percentage of a core usage over time interval

Memory measurements: (Linux only)
 + **averageResidentSetSize** - average resident set size used by process (expressed in kB)
 + **averageVirtualMemorySize** - average virtual memory used by process (expressed in kB)

### StdOut backend output format
```
[METRIC] <name>,<type> <values> <timestamp> <tags>
```
The prefix (`[METRIC]`) can be changed using query component.

### Regex verbosity policy
Overwrite metric verbosity using regex expression:
```
Metric::setVerbosityPolicy(Verbosity verbosity, const std::regex& regex)
```

## System monitoring, server-side backends installation and configuration
This guide explains manual installation. For `ansible` deployment see [AliceO2Group/system-configuration](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) gitlab repo.
