# Monitoring
Monitoring module allows to inject user defined metrics and monitor the process itself. It supports multiple backends, protocols and data formats.

## aliBuild installation
Install [aliBuild](https://alisw.github.io/alibuild/) first.
~~~
pip install alibuild
~~~
Then follow instructions below.
~~~
aliBuild init Monitoring@master
aliDoctor Monitoring
aliBuild --defaults o2 build Monitoring
alienv load Monitoring/latest
~~~

## Manual installation
Manual installation of the O<sup>2</sup> Monitroing module and its dependencies.

### Boost
It is assumed that Boost is present in your system. For more information see [Boost Getting Started](http://www.boost.org/doc/libs/1_63_0/more/getting_started/unix-variants.html) page.

### O<sup>2</sup> Configuration module
~~~
git clone https://github.com/AliceO2Group/Configuration.git
cd Configuration; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
~~~

See [README](https://github.com/AliceO2Group/Configuration#manual-installation) if you're missing any Configuration dependencies.

### libcurl (optional)
It should be present in your system or available in package manager, otherwise go to [curl download page](see: https://curl.haxx.se/download.html).

### ApMon (optional)
~~~
wget http://monalisa.caltech.edu/download/apmon/ApMon_cpp-2.2.8.tar.gz
./configure --prefix=<installdir>
make -j
make install
~~~

### Monitoring module
~~~
git clone https://github.com/AliceO2Group/Monitoring.git
cd Monitoring; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
~~~

## Metrics
Metrics consist of 4 parameters: name, value, timestamp and tags.I

| Parameter name | Type                                          | Required | Default          |
| -------------- |:---------------------------------------------:|:--------:| ----------------:|
| name           | string                                        | yes      | -                |
| value          | int / double / string / unsigned long long    | yes      | -                |
| timestamp      | chrono::time_point&lt;std::chrono::system_clock&gt; | no       | current timestamp     |
| tags           | vector<Tag>                                   | no       | -**                |

**Default tag set is process specific and included in each metric:
+ hostname
+ PID
+ process name

See how backends handle tags in  [Monitoring backends](#monitoring-backends) section.

## Creating monitring instance
1. The recommended way of getting (reference to) monitoring instance is via *MonitoringFactory*.
Before using the factory *Configure* method must be called providing URI to configuration file or backend. It should be called only once per process (following calls will not have any effect).
```cpp
AliceO2::Monitoring::MonitoringFactory::Configure("file://../Monitoring/examples/SampleConfig.ini");
AliceO2::Monitoring::MonitoringFactory::Get();
```

2. A second way allows to create dedicated monitoring instance. It's useful only when different configuration URI is needed within the same process.
```cpp
Monitoring::Create("file://../Monitoring/examples/SampleConfig.ini");
```

## Sending a metric
Metric can be sent by one of following ways:
1. By creating and moving metric object:
   + `send(Metric&& metric)`

   In addition, two additional methods can be chained:
   + `addTags(std::vector<Tag>&& tags)`
   + `setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp)`

2. By providing metric parameters:
   + `send(T value, std::string name)`
   + `sendTagged(T value, std::string name, std::vector<Tag>&& tags)`
   + `sendTimed(T value, std::string name, std::chrono::time_point<std::chrono::system_clock>& timestamp)`

See [Getting started](#getting-started) section to learn more.

## Derived metrics
The module can also calculate derived metrics. To do so, use `addDerivedMetric(std::string name, DerivedMetricMode mode)` with one of two available modes:
+ `DerivedMetricMode::RATE` - rate between two following metrics;
+ `DerivedMetricMode::AVERAGE` - average value of all metrics stored in cache;

Derived metrics are generated each time as new value is passed to the module. Their names are suffixed with derived mode.

## Processes monitoring
To enable process monitoring *ProcessMonitor.enable* flag in configuration file must be set to 1 - see [Configuration file](#configuration-file) section. The following metrics are generated every N seconds (N can be specified in the config - *ProcessMonitor.interval*):
+ **etime** - elapsed time since the process was started, in the form [[DD-]hh:]mm:ss
+ **pcpu** - cpu utilization of the process in "##.#" format. Currently, it is the CPU time used divided by the time the process has been running (cputime/realtime ratio), expressed as a percentage.  It will not add up to 100% unless you are lucky
+ **pmem** - ratio of the process's resident set size  to the physical memory on the machine, expressed as a percentage
+ **bytesReceived** - the total number of bytes of data received by the process (per interface)
+ **bytesTransmitted** - the total number of bytes of data transmitted by the process (per interface).

## Monitoring backends
Metrics are pushed to one or multiple backends. The module currently supports three backends - see table below. Enabling/Disabling backends is done via configuration file.

| Backend name     | Description                    | Transport                    | Client-side requirements   | Handling tags  |
| ---------------- |:------------------------------:|:----------------------------:|:--------------------------:| --------------:|
| InfluxDB         | InfluxDB time series database  | HTTP / UDP (InfluxDB Line Protocol) | cURL / boost asio   | Supported by InfluxDB Line Protocol |
| ApMonBackend     | MonALISA Serivce               | UDP                          | ApMon                      | Default tags concatenated with entity; Metric tags concatenated with name |
| InfoLoggerBackned| O2 Logging module              | -                            | (as log message)           | Added to the end of message |
| Flume            | Collects, aggragate monitoring data | UDP (JSON)              | boost asio                 | In Flume Event header |

Instruction how to install and configure server-sides backends are available in [Server-side backend installation and configuration](#server-side-backend-installation-and-configuration) section.

## Configuration file
+ AppMon
  + enable - enable AppMon (MonALISA) backend
  + pathToConfig - path to AppMon configuration file
+ InfluxDB
  + enableUDP - enable InfluxDB UDP endpoint
  + enableHTTP - enable InfluxDB HTTP endpoint
  + hostname - InfluxDB hostname
  + port - InfluxDB port
  + db - name of database
+ InfoLoggerBackend
  + enable - enable InfoLogger backend
+ Flume
  + enable - enable Flume HTTP backend
  + port 
  + hostname
+ ProcessMonitor
  + enable - enable process monitor
  + interval - updates interval[]
+ DerivedMetrics
  + maxCacheSize - maximum size of vector

See sample in [examples/config-default.ini](https://github.com/awegrzyn/Monitoring/blob/master/examples/config-default.ini).

## Getting started
Examples are available in [example](https://github.com/awegrzyn/Monitoring/tree/master/examples) directory.

### Sending user defined metric - examples/1-Basic.cxx
```cpp
// configure monitoring (once per process), pass configuration path as parameter
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

// now send an application specific metric
// 10 is the value
// myMetric is the name of the metric
//  
// 1. by copying values
Monitoring::Get().send(10, "myMetric");
  
// 2. by creating and moving metric object
Monitoring::Get().send({10, "myMetric"});
```

### Sending tagged metric - examples/2-TaggedMetrics.cxx
```cpp
// configure monitoring (only once per process), pass configuration path as parameter
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

// now send an application specific metric with additional tags
// 10 is the value
// myMetric is the name of the metric
// then vector of key-value tags
//  
// 1. by copying value and name and moving tags
Monitoring::Get().sendTagged(10, "myMetric", {{"tag1", "value1"}, {"tag2", "value2"}});

// 2. by moving value, name and tags
Monitoring::Get().send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}))
```

### Sending metric with user defined timestamp - examples/3-UserDefinedTimestamp.cxx
By default timestamp is set by the module, but user can overwrite it manually.
```cpp
// configure monitoring (only once per process), pass configuration path as parameter
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

// current timestamp
std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
     
// now send an application specific metric
// 10 is the value timestamp
// myCrazyMetric is the name of the metric
//  
// 1. by copying all parameters
Monitoring::Get().sendTimed(10, "myCrazyMetric", timestamp);
std::this_thread::sleep_for(std::chrono::seconds(1));
Monitoring::Get().sendTimed(40, "myCrazyMetric", timestamp);
  
  // 2. by moving
Monitoring::Get().send(Metric{10, "myCrazyMetric"}.setTimestamp(timestamp));
std::this_thread::sleep_for(std::chrono::seconds(1));
Monitoring::Get().send(Metric{40, "myCrazyMetric"}.setTimestamp(timestamp));
```

###  Derived metrics - examples/4-RateDerivedMetric.cxx
The module can calculate derived metrics: average value and rate.
```cpp
// configure monitoring (only once per process), pass configuration path as parameter
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

// derived metric :  rate
Monitoring::Get().addDerivedMetric("myMetric", AliceO2::Monitoring::DerivedMetricMode::RATE);

// now send at least two metrics to see the result
Monitoring::Get().send(10, "myMetric");
Monitoring::Get().send(20, "myMetric");
Monitoring::Get().send(30, "myMetric");
Monitoring::Get().send(50, "myMetric");
```

### Dedicated monitoring instance - examples/6-DedicatedInstance.cxx
```cpp
// create dedicated monitoring instance, pass confuguration path as parameter
auto collector = Monitoring::Create("file:///home/awegrzyn/hackathon/Monitoring/examples/SampleConfig.ini");

// now send an application specific metric
// 10 is the value
// myMetric is the name of the metric
//  
// 1. by copying values
collector->send(10, "myMetric");
  
// 2. by creating and moving metric object
collector->send({10, "myMetric"});
```

## Server-side backend installation and configuration

### MonALISA Service
To install and configure the MonALISA service (1 central server):
+ `yum install monalisa-service`
+ copy `config/ml_env` to `/opt/monalisa-service/Service/CMD/`
+ copy `config/ml.properties` to `/opt/monalisa-service/Service/myFarm/`
+ copy `config/myFarm.conf` to `/opt/monalisa-service/Service/myFarm/`
+ add following line to iptables and restart it: `-A INPUT -p udp -m state --state NEW -m udp --dport 8884 -m comment --comment "MonALISA UDP packets" -j ACCEPT`
+ `/sbin/service MLD start`

### InfluxDB
Instructions are available at [InfluxDB page](https://docs.influxdata.com/influxdb/v1.2/introduction/installation/).

### InfoLogger
The InfoLogger module is shared library which is linked during compilation time.

### Flume
See [Flume User Guide](https://flume.apache.org/FlumeUserGuide.html) documentation.
