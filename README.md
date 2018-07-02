# Monitoring
[![travis-ci](https://api.travis-ci.org/AliceO2Group/Monitoring.svg?branch=master)](https://travis-ci.org/AliceO2Group/Monitoring)
[![aliBuild](https://img.shields.io/badge/aliBuild-dashboard-lightgrey.svg)](https://alisw.cern.ch/dashboard/d/000000001/main-dashboard?orgId=1&var-storagename=All&var-reponame=All&var-checkname=build%2FMonitoring%2Fo2-dataflow%2F0&var-upthreshold=30m&var-minuptime=30)
[![JIRA](https://img.shields.io/badge/JIRA-issues-blue.svg)](https://alice.its.cern.ch/jira/projects/OMON)

Monitoring module allows to inject user defined metrics and monitor the process itself. It supports multiple backends, protocols and data formats.

## Table of contents
1. [Installation](#installation)
2. [Getting started](#getting-started)
3. [Features and additional information](#features-and-additional-information)
4. [Code snippets](#code-snippets)
5. [System monitoring and server-side backends installation and configuration](#system-monitoring-server-side-backends-installation-and-configuration)

## Installation
### RPM (CentOS 7 only)
+ Install CERN certificates
~~~
yum -y install CERN-CA-certs
~~~

+ Add `alisw` repo **(as root)**
~~~
cat > /etc/yum.repos.d/alisw-el7.repo <<EOF
[alisw-el7]
name=ALICE Software - EL7
baseurl=https://ali-ci.cern.ch/repo/RPMS/el7.x86_64/
enabled=1
gpgcheck=0
EOF
~~~

+ Install Monitoring RPM package **(as root)**
~~~
yum -y install alisw-Monitoring+v1.5.4-1.x86_64
~~~

+ Configure Modules
~~~
export MODULEPATH=/opt/alisw/el7/modulefiles:$MODULEPATH
~~~

+ Load enviroment
~~~
eval `modulecmd bash load Monitoring/v1.5.4-1`
~~~
The installation directory is: `/opt/alisw/el7/Monitoring/v1.5.4-1`

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

~~~
git clone https://github.com/AliceO2Group/Monitoring.git
cd Monitoring; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
~~~

## Getting started
### Monitoring instance
The recommended way of getting (`unique_ptr` to) monitoring instance is `Get`ing it from  `MonitoringFactory` by passing backend URI(s) as a parameter (comma seperated if more than one).
The library is accessible from `o2::monitoring` namespace.

```cpp
#include <MonitoringFactory.h>
using namespace o2::monitoring;
std::unique_ptr<Monitoring> monitoring = MonitoringFactory::Get("backend[-protocol]://host:port[?query]");
```
See table below to find out how to create `URI` for each backend:

| Backend name | Transport | URI backend[-protocol] | URI query        |
| ------------ |:---------:|:----------------------:| ----------------:|
| InfluxDB     | HTTP      | `influxdb-http`        | `/write?db=<db>` |
| InfluxDB     | UDP       | `influxdb-udp`         | -                |
| ApMon        | UDP       | `apmon`                | -                |
| InfoLogger   | -         | `infologger`           | -                |
| Flume        | UDP       | `flume`                | -                |

Multiple backends may be used at the same time, URLs should be separated by `,` (comma).

### Sending metric

```cpp
send(Metric&& metric, [DerivedMetricMode mode])
```
Where metric constructor receives following parameters:
  - `T value`
  - `std::string& name`
  - `[time_point<system_clock> timestamp]`

For example:
```cpp
monitoring->send({10, "myMetricInt"});
```

Regarding  `DerivedMetricMode` see [Calculating derived metrics](#calculating-derived-metrics).

### Customized metrics
Two additional methods can be chained the to `send(Metric&& metric)` in order to __insert custom tags__ or __set custom timestamp__:
   + `addTags(std::vector<Tag>&& tags)`
   + `setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp)`

For example:
```cpp
monitoring->send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
monitoring->send(Metric{10, "myCrazyMetric"}.setTimestamp(timestamp));
```

## Features and additional information

### Grouped values
It's also possible to send multiple, grouped values in a single metric (`Flume` and `InfluxDB` backends are supproted, others fallback into sending values in seperate metrics)
```cpp
void sendGroupped(std::string name, std::vector<Metric>&& metrics)
```

For example:
```cpp
monitoring->sendGroupped("measurementName", {{20, "myMetricIntMultiple"}, {20.30, "myMetricFloatMultple"}});
```

### Buffering metrics
In order to avoid sending each metric separately, metrics can be temporary stored in the buffer and flushed at the most convenient moment.
This feature can be operated with following two methods:
```cpp
monitoring->enableBuffering(const unsigned int maxSize)
...
monitoring->flushBuffer();
```

`enableBuffering` takes maximum buffer size as its parameter. The buffer gets full all values are flushed automatically.

For example:
```cpp
monitoring->enableBuffering(5);
for (int i = 1; i < 10; i++) {
  monitoring->send({10, "myMetricInt"});
}
monitoring->send({20, "myMetricInt2"});
monitoring->flushBuffer();
```

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

## Code snippets
Code snippets are available in [examples](examples/) directory.

 + Sending metric - [examples/1-Basic.cxx](examples/1-Basic.cxx)
 + Sending metric with custom taggs - [examples/2-TaggedMetrics.cxx](examples/2-TaggedMetrics.cxx)
 + Sending metric with user defined timestamp - [examples/3-UserDefinedTimestamp.cxx](examples/3-UserDefinedTimestamp.cxx)
 + Calculating derived metrics - [examples/4-RateDerivedMetric.cxx](examples/4-RateDerivedMetric.cxx)
 + Sending multiple values in a single metric - [examples/8-Multiple.cxx](examples/8-Multiple.cxx)

## System monitoring, server-side backends installation and configuration
This guide explains manual installation. For `ansible` deployment see [AliceO2Group/system-configuration](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) gitlab repo.

 + [Collectd](doc/collectd.md)
 + [Flume](doc/flume.md)
 + [InfluxDB](doc/influxdb.md)
 + [Grafana](doc/grafana.md)
 + [MonALISA](http://monalisa.caltech.edu/monalisa__Documentation__Service_Installation_Guide.html) (external link)
