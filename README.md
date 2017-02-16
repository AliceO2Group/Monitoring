# Monitoring module

Monitoring module allows to:
+ inject user specific metrics
+ monitor process itself (cpu / memory)

## Metrics
Metrics consist of 4 parameters: name, value, timestamp and tags.I

| Parameter name | Type                                          | Required | Default          |
| -------------- |:---------------------------------------------:|:--------:| ----------------:|
| name           | string                                        | yes      | -                |
| value          | int/double/uint32t/string                     | yes      | -                |
| timestamp      | chrono::time_point&lt;std::chrono::system_clock&gt; | no       | current timestamp     |
| tags           | vector<Tag>                                   | no       | -**                |

**Default tag set is generated per process that attached to each sent metric:
+ hostname
+ PID
+ process name

See how backends are handling tags in  *Monitoring backends* section.

## Creating monitring instance
The most recommended way of getting (reference to) monitoring instance is via *MonitoringFactory*.
Before using the factory Configure method must be call providing path to cofiguration file/backend. It should be called only once per process (following calls will not have any effect).
```cpp
AliceO2::Monitoring::MonitoringFactory::Configure("file://../Monitoring/examples/SampleConfig.ini");
AliceO2::Monitoring::MonitoringFactory::Get();
```

The second way is getting dedicated instance. It should be used only when different configuration path needs to be passed within the same process.
```cpp
Monitoring::Create("file://../Monitoring/examples/SampleConfig.ini");
```

See *Getting started* before writting any code.

## Sending metric
Metric can be sent by one of following ways:
+ By creating and moving metric object: *send(Metric&& metric)*. In addition there are two methods that can be invoked on Metric object:
   + *addTags(std::vector&lt;Tag&gt;&& tags)*
   + *setTimestamp(std::chrono::time_point&lt;std::chrono::system_clock& &gt;timestamp)*
+ By providing all required metric parameters (name and value): *send(T value, std::string name)*. In addition there are two suplementary methods:
   + *sendTagged(T value, std::string name, std::vector&lt;Tag&gt;&& tags)*
   + *sendTimed(T value, std::string name, std::chrono::time_point&lt;std::chrono::system_clock&gt;& timestamp)*

See *Getting started* section to learn usage.

## Derived metrics
Library can calculate Derived metrics - use  *void addDerivedMetric(std::string name, DerivedMetricMode mode)* with one of two available modes:
+ *DerivedMetricMode::RATE* - rate between two following metrics;
+ *DerivedMetricMode::AVERAGE* - average value of all metrics stored in cache;

Derived metrics are generated as new value arrives. 
Derived metric will have suffix added to its name.

## Processes monitoring
To enable process monitoring *ProcessMonitor.enable* flag in configuration file must be set to 1 (see section Configuration file).
The following metrics are generated every N seconds (N can be specified in the config - *ProcessMonitor.interval*):
1. etime - elapsed time since the process was started, in the form [[DD-]hh:]mm:ss.
2. pcpu - cpu utilization of the process in "##.#" format. Currently, it is the CPU time used divided by the time the process has been running (cputime/realtime ratio), expressed as a percentage.  It will not add up to 100% unless you are lucky.
3. pmem - ratio of the process's resident set size  to the physical memory on the machine, expressed as a percentage.

## Monitoring backends
Metrics are pushed to one or multiple backends. The library currently supports three backends - see table below.

| Backend name     | Description                    | Transport                    | Client-side requirements   | Handling tags  |
| ---------------- |:------------------------------:|:----------------------------:|:--------------------------:| --------------:|
| InfluxDB         | InfluxDB time series database  | HTTP / UDP                   | cURL / boost asio          | Supported by the backend |
| ApMonBackend     | MonALISA Serivce               | UDP                          | ApMon                      | Default tags concatenated with entity; Metric tags concatenated with name |
| InfoLoggerBackned| O2 Logging module              | -                            | -                          | Displayed in the end |

Instruction how to install and configure server-sides backends are available in "Server-side backend installation and configuration" chapter.

## Configuration file
+ AppMon
  + enable - enable AppMon (MonALISA) backend
  + pathToConfig - path to AppMon configuration file
+ InfluxDB
  + enable - enable InfluxDB backend
  + hostname - server hostname
  + port - server port
  + db - name of database
+ InfoLoggerBackend
  + enable - enable InfoLogger backend
+ ProcessMonitor
  + enable - enable process monitor
  + interval - updates interval 
+ DerivedMetrics
  + maxCacheSize - maximum size of vector

See sample in examples/SampleConfig.ini

## Getting started
Examples are available in *example* directory.

### Sending user defined metric - examples/1-Basic.cxx
```cpp
// create monitoring object, pass confuguration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/hackathon/Monitoring/SampleConfig.ini")
  );  

  // now send an application specific metric
  // 10 is the value
  // myMetric is the name of the metric
  //  
  // 1. by copying values
  collector->send(10, "myMetric");
  
  // 2. by creating and moving metric object
  collector->send({10, "myMetric"});
```

### Sending tagged metric - examples/2-TaggedMetrics.cxx
```cpp
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/hackathon/Monitoring/SampleConfig.ini")
  );
  
  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then vector of key-value tags
  //  
  // 1. by copying value and name and moving tags
  collector->sendTagged(10, "myMetric", {{"tag1", "value1"}, {"tag2", "value2"}});

  // 2. by moving value, name and tags
  collector->send(Metric{10, "myMetric"}.addTags({{"tag1", "value1"}, {"tag2", "value2"}}));
```

### Sending metric with user defined timestamp - examples/
By default timestamp is set by the library, but user can overwrite it manually.
```cpp
  // create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/hackathon/Monitoring/SampleConfig.ini")
  );  

  // current timestamp
  std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
     
  // now send an application specific metric
  // 10 is the value timestamp
  // myCrazyMetric is the name of the metric
  //  
  // 1. by copying all parameters
  collector->sendTimed(10, "myCrazyMetric", timestamp);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  collector->sendTimed(40, "myCrazyMetric", timestamp);
  
  // 2. by moving
  collector->send(Metric{10, "myCrazyMetric"}.setTimestamp(timestamp));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  collector->send(Metric{40, "myCrazyMetric"}.setTimestamp(timestamp));
}
```

###  Derived metrics
The Library can calculate derived metrics: average value and rate.
```cpp
// create monitoring object, pass configuration path as parameter
  std::unique_ptr<Monitoring::Collector> collector(
    new Monitoring::Collector("file:///home/hackathon/Monitoring/SampleConfig.ini")
  );  

  // derived metric :  rate
  collector->addDerivedMetric("myMetric", Monitoring::DerivedMetricMode::RATE);

  // now send at least two metrics to see the result
  collector->send(10, "myMetric");
  collector->send(20, "myMetric");
  collector->send(30, "myMetric");
  collector->send(50, "myMetric");
```

## Server-side backend installation and configuration

### MonALISA
##### MonALISA Service
To install and configure the MonALISA service (1 central server):
+ yum install monalisa-service
+ copy config/ml_env to /opt/monalisa-service/Service/CMD/
+ copy config/ml.properties to /opt/monalisa-service/Service/myFarm/
+ copy config/myFarm.conf to /opt/monalisa-service/Service/myFarm/
+ add following line to iptables and restart it: -A INPUT -p udp -m state --state NEW -m udp --dport 8884 -m comment --comment "MonALISA UDP packets" -j ACCEPT
+ /sbin/service MLD start

##### MonALISA Client
To install the MonALISA Client run:
+ yum install monalisa-client

### InfluxDB
Instructions are avaliable here: https://docs.influxdata.com/influxdb/v1.2/introduction/installation/

### InfoLogger
There is no installation/configuration procedure needed. InfoLogger backend is available all the time (if not disabled in configuration file).
