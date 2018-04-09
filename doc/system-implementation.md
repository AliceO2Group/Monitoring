# Monitoring system implementation based on Modular Stack

## 1. Goal and requirements

The goal of the monitoring system is to provide experts and shift crew in the ALICE Control Centre with an in-depth state of the O<sup>2</sup> computing farm. The near-real-time and historical graphical dashboards interface with users providing outstanding experience and allow grasp easily large amount of monitoring data.
In order to supply data to the end user the set of tools has been selected in the official evaluation to meet the requirements specified in the [O<sup>2</sup> Technical Design Report](https://cds.cern.ch/record/2011297/files/ALICE-TDR-019.pdf) and defined by O2 Work Package 8:
- Compatible with the O<sup>2</sup> reference operating system (currently CERN CentOS 7).
- Well documented.
- Actively maintained and supported by developers.
- Run in isolation when external services and/or connection to outside of ALICE are not available.
- Capable of handling 600 kHz input metric rate.
- Scalable to >> 600 kHz if necessary.
- Handle at least 100 000 sources.
- Introduce latency no higher than 500 ms up to the processing layer, and 1000 ms to the visualisation layer.
- Impose low storage size per measurement.
- Aligned with functional architecture:
  - Sensors.
  - Metric processing.
  - Historical record and near-real-time visualisation.
  - Alarming.
  - Storage that supports down-sampling, large input metric rates and low storage size.

  In addition, some optional requirements may positively influence the final rating:
  -	Supported by CERN or used in one of the experiments/departments.
  -	Self-recovery in case of connectivity issues.

## 2. Functional architecture

Figure 1 shows the functional architecture of the system. Three monitoring data sources has been identified:
- Application (O<sup>2</sup> related)
- Process
- System.

![](images/gen_arch.png)

<p align="center">Figure 1. Monitoring architecture</p>

These data sources send the monitoring data periodically to the server-side processing and aggregation task which can perform stream or batch operations such as:
- Suppression
- Enrichment
- Correlation
- Custom aggregation and others.

Afterwards data are forwarded to both storage and real-time dashboard. The selected storage must be support high input metric rate, low storage size and downsampling. The near-real-time dashboard receives selected, processed metrics as soon as it is possible in order to allow experts to react to abnormal situations. This imposes a need of low latency transport layer.
The historical dashboard displays data from the storage. As it has access to larger variety of metrics it is mostly used by experts in order to drill down the issues and access detailed views.
These dashboards display data as time series plots, gauges, bar, and other graphical objects. They allow access from various operating systems and from outside of the experimental area (Point 2).
Eventually, the alarming component detects abnormal situations and notifies experts in form on text message or other mean of notification.

## 3. The Modular Stack
The Modular Stack solution aims at fulfilling the requirements specified in the Section 1 by using multiple and replaceable tools. Such approach enables the possibility of replacing one or more of the selected components in case alternative options provide improved performance or additional functionalities.
This section gives an overview of Modular Stack components, see Figure 2, while the detailed description can be found in the following chapters.

![](images/monsta_arc.png)

<p align="center">Figure 2. Modular Stack architecture</p>     

The O<sup>2</sup> Modular Stack collects three classes of metrics (as defined in Section 2) with assistance of [O<sup>2</sup> Monitoring library](http://github.com/AliceO2Group/Monitoring) (Application and process metrics)
and [CollectD](http://collectd.org/) (system information). Collectd deploys system sensors to collect metrics related to CPU, memory and I/O from all O<sup>2</sup> nodes. The requirements of high monitoring metric rate collection and metric routing is address via [Apache Flume](https://flume.apache.org/), "a distributed and highly-reliable service for collecting, aggregating and moving large amounts of data in a very efficient way". Moreover, Flume can also run simple processing tasks. [InfluxDB](https://docs.influxdata.com/influxdb/v1.5/), "a custom high-performance data store written specifically for time series data", was selected as a storage. [Grafana](https://grafana.com/) provides graphical interfaces to display near real-time and historical metrics. [Riemann](http://riemann.io/) triggers alarms using multiple plugins and allows to implement some processing tasks. All remaining and complex processing tasks are implemented through [Apache Spark](https://spark.apache.org/), "a fast and general-purpose engine for large-scale data processing".

### 3.1 Sensors and data sources
As explained in the Section 2 three classes of metrics are collected using O<sup>2</sup> Monitoring library and CollectD. This section provide more details regarding these tools.

#### 3.1.1 Monitoring library - application and process performance metrics

The [O<sup>2</sup> Monitoring library](https://github.com/AliceO2Group/Monitoring) provides an entry point for the O<sup>2</sup> processes to the Monitoring subsystem. It forwards user defined and [process performance](https://github.com/AliceO2Group/Monitoring/blob/dev/README.md#monitoring-process) related metrics to one of the [supported backends](https://github.com/AliceO2Group/Monitoring/blob/dev/README.md#monitoring-instance). It allows calculation of [derived metrics](https://github.com/AliceO2Group/Monitoring/blob/dev/README.md#calculating-derived-metrics) (eg. rate). It also supports [metric buffering](https://github.com/AliceO2Group/Monitoring/blob/dev/README.md#buffering-metrics) in order to improve the efficiency metric transport and decrease number context switches.

The detailed description of the library in available in the [README](https://github.com/AliceO2Group/Monitoring/blob/dev/README.md) file.

#### 3.1.2 Collectd - system metrics
The following collectD plugins has been selected in order to provide system overview of each node:

| Plugin    | Metric description | Requirements | Comments |
| --------- | ------------------ | ------------ | -------- |
| CPU       | Amount of time spent by the CPU in various states (user, system, io, idle, irq) | `/proc` | Jiffie unit |
| Interface | Throughput, packets/second and errors/second per interface | `/proc` | List of interfaces can be defined |
| Memory    | Memory utilisation (Used, Buffered, Cached, Free) | `/proc` | - |
| DF        | Used and available disk space per mounted partition | `statfs`, `getmntent` | List of partitions can be defined |
| Load      | System load as 1, 5, 15 minutes average | `/proc` | - |
| Uptime    | Execution time; current, average and maximum | `/proc` | - |
| Disk      | Disk performance metrics per read and write: average time, operation per second, io time and more | `/proc/diskstats` | - |
| Logfile   | Internal; writes collectd logs into a file | - | - |

Data from collectd can be transferred by one of two plugins:
 - [Network](https://collectd.org/wiki/index.php/Plugin:Network) - binary protocol over UDP (natively supported by InfluxDB)
 - [Write HTTP](https://collectd.org/wiki/index.php/Plugin:Write_HTTP) - Formats data JSON and send over HTTP (supported by custom-made Flume handler)

### 3.2 Apache Flume - Collection and routing
The large amount of monitoring data generated from the O<sup>2</sup> Facility requires a high-performance collection and aggregation. The  goal of this component is to receive monitoring data from the sensors (collectD) and O<sup>2</sup> processes (Monitoring library) and route them towards one of the following tools (as shown in Figure 2):
- Historical storage (InfluxDB)
- Near real-time dashboard (Grafana)
- Alarming (Riemann)
- Processing (Apache Spark).

![](images/flume-arc.png)
<p align="center">Figure 3. Flume agent (from Apache Flume official documentation)</p>

A Flume instance runs an agent, which is "process that hosts the components through which events flow from an external source to the next destination (hop)". An agent consists of following components (see Figure 3):
- Source - Receives metrics and formats them into an Flume event
- Channel - Disk or memory buffer
- Channel selector - selects an outgoing channel or channels based on event properties such as a metric name or a dedicated property for this purpose; this property could be set either in a source interceptor or by the monitoring library.
- Sink - Translates Flume events into desirable format and sends them to a backend
- Interceptor - Basic manipulation on Flume events
- Handler - Stream changes to captured data by a source

Apache Flume provides wide range of built-in components eg. HTTP source, Avro sink and source (to communicate with Spark). In order to interface with all Modular Stack tools some components needs to developed:
- [InfluxDB Sink](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-udp-influxdb-sink) - pushes events to InfluxDB via UDP
- Grafana Real-Time Sink (implementation details not yet defined)
- Riemann Sink (implementation details not yet defined)
- [UDP/JSON Source](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-udp-source) - collects metrics sent from the [O<sup>2</sup> monitoring library](https://github.com/AliceO2Group/Monitoring) via UDP as JSON encoded strings
- [Collectd JSON Handler](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-json-collectd-http-handler) - reads the data provided by collectd [write_http](https://collectd.org/wiki/index.php/Plugin:Write_HTTP) plugin.

The Figure 2 shows how the components are connected with each other.

![](images/flume_inner_arc.png)
<p align="center">Figure 4. Flume internal architecture</p>

#### 3.2.1 Flume Event
Flume transports metrics in the data structure called Event, which consists of:
- byte array payload
- set of string attributes (optional).

It was decided to keep metric name, values, tags and timestamp as string attributes and leave array payload empty. In addition, tags names are prefixed with `tag_` and value names with `value_`. Therefore, each Flume Event consists of following string attributes:
- `name`
- `timestamp` - UNIX epoch time format
- `tag_host` - hostname of an origin of a metric
- `value_*` - at least one value is required
- [`tag_*`] - optional tags

#### 3.2.2 UDP/JSON Source

The UDP/JSON Source receives metrics from the Monitoring library. The UDP protocol was chosen as it allows receiving data from thousands of nodes. In addition, it drops the data (instead of queuing them) when the receiving part is busy, what is desired behaviour.
There can be one or more metrics in a single UDP packet. The JSON metric has format similar to Flume Event what eases the parsing.
A sample metric looks like below:
```JSON
[{
  "headers" : {
    "timestamp" : "434324343",
    "tag_host" : "o2.cern.ch",
    "name" : "cpu",
    "value_idle" : "0.13"
  },
  "body" : ""
}]
```

The information how build and configure UDP/JSON Source are provided in the [GitHub README](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-udp-source).

#### 3.2.3 CollectD JSON Handler

The goal of the CollectD JSON Handle is to make collect data coming from CollectD daemons possible. As previously mentioned in the Section 3.1.2 CollectD sends metrics either as binary blobs via UDP or as JSON via HTTP. The latter option was selected in order to interface with the source as it eases parsing to Flume Event. The performance should not be concern in this case as the source needs to handle ~200 requests per second (2000 collectD daemons pushing data every 10 seconds).

Below, two metrics from `disk` plugin, send over HTTP by `http_write` plugin are shown:

```JSON
[{ "values": [197141504, 175136768],
   "dstypes": ["counter", "counter"],
   "dsnames": ["read", "write"],
   "time": 1251533299.265,
   "interval": 10,
   "host": "o2.cern.ch",
   "plugin": "disk",
   "plugin_instance": "sda",
   "type": "disk_octets",
   "type_instance": ""
   }
]
```

The `values`, `dstypes` and `dsname` fields are arrays of the same size allowing to accommodate multiple metrics. In the example, two values, types and names are specified.

The `dstypes` field defines the value type:
- `counter` or `absolute` - unsigned integer
- `derive` - signed integer
- `gauge` - double.


Flume natively provides HTTP Source but it is not able to decode the CollectD JSON format. The handler was implemented to provide such functionality. It creates separate Flume Event per each metric specified in the given position of `values`, `dstypes` and `dsname` arrays.

The following Flume Event headers are defined:
- `timestamp` - `time` converted into nanoseconds (`long` type)
- `tag_host` - `host` value
- `name` - `plugin` name concatenated with `dsnames` eg. `disk_read`, `disk_write`.
- `value_value` - given `dstypes` value
- [`type_value`] - `long` for `counter`, `absolute` types and `double` for `gauge` type
- `tag_plugin_instance` - `plugin_instance` value
- `tag_type` - `type` value

Following the above actions, the CollectD JSON shown before produces the following Flume Event:
```JSON
[{"headers" : {
    "timestamp" : "1251533299265000000",
    "tag_host" : "o2.cern.ch",
    "name" : "disk_read",
    "value_value":  "197141504",
    "type_value" : "long",
    "tag_plugin_instance": "sda",
    "tag_type": "disk_octets"
    },
  "body" : ""
  },
 {"headers" : {
    "timestamp" : "1251533299265000000",
    "tag_host" : "o2.cern.ch",
    "name" : "disk_write",
    "value_value" : "175136768",
    "type_value" : "long",
    "tag_plugin_instance": "sda",
    "tag_type": "disk_octets"
    },
  "body" : ""
  }
]
```

The information how build and configure CollectD JSON Handler are provided in the [Github README](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-json-collectd-http-handler).


#### 3.2.4 Spark Avro Source
[Avro Source](http://flume.apache.org/FlumeUserGuide.html#avro-source) receives data from Apache Spark. Alternatively, it is possible to use UDP/JSON Source for this purpose.

#### 3.2.5 InfluxDB Sink
The InfluxDB Sink takes an Flume Event from a channel, converts it to the [InfluxDB Line Protocol](https://docs.influxdata.com/influxdb/v1.5/write_protocols/line_protocol_reference/) and finally sends it in the UDP packet. The alternative solution of using HTTP requests was rejected due to a performance.

The InfluxDB Line Protocol format is following:
```
"metric_name,tag1=key1,tag2=key2 value_name1=value1,value_name2=value2 timestamp_ns"
```

A value has 1 of 4 types:
-	Long - value suffixed with `i` character, example: `count=10i`.
-	Double - default, example: `idle=0.98`.
-	String - value quoted, example: `hostname=\"test-machine\"`.
-	Boolean - `true` or `false`, example: `ping=true`.

For Example, the following Flume Event:
```JSON
[{"headers" : {
    "timestamp" : "434324343",
    "tag_host" : "o2.cern.ch",
    "tag_cpu" : "1",
    "tag_site" : "CNAF",
    "name" : "cpu",
    "value_idle" : "0.93",
    "value_user" : "0.03"
    },
  "body" : ""
  }
]
```

produces the following InfluxDB Line Protocol string:
```
"cpu,host=o2.cern.ch,site=CNAF,cpu=1 idle=0.93,user=0.03 434324343"
```

The information how build and configure InfluxDB Sink are provided in the  [Github README](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-udp-influxdb-sink)


#### 3.2.6 Grafana Sink
The near real-time Grafana Sink sends the data directly to the near real-time dashboard with a minimum latency.

The component has not been developed yet.

#### 3.2.7 Riemann Sink
The Riemann Sink sends the data to the Riemann instance in order to trigger a notification.
Riemann accepts both HTTP and TCP.

The component has not been developed yet.

#### 3.2.8 Spark Avro Sink
The Spark Sink sends the data to Spark for further processing and aggregation. According to the [Spark Streaming - Flume integration documentation](https://spark.apache.org/docs/2.2.0/streaming-flume-integration.html) there are two approaches of sending data to Spark:
- Push-based
- Pull-based.

In the first approach, Spark acts like a Flume Avro Source, thus the Spark Sink simply becomes built-in [Avro Sink](http://flume.apache.org/FlumeUserGuide.html#avro-sink).
In the second approach, the Spark Sink buffers the events while "Spark uses a reliable Flume receiver and transactions to pull data from the sink".

Currently Push is used as Pull approach has some undesired effects:
- When Spark job restarts it begins processing the last retrieved data, even if it's already outdated; the rule "the new data has more priority than old" is followed
- The time windows are not fixed.


#### 3.2.9 Information Enrichment Interceptor
The information Enrichment Interceptor adds additional string attributes to specific Flume events.
Currently, only [InfluxDB timestamp interceptor](https://github.com/AliceO2Group/MonitoringCustomComponents/tree/master/flume-influxdb-timestamp-interceptor) is available.

### 3.3 Apache Spark - Batch and streaming processing
The defined processing tasks are:
- enrichment
- aggregation
- correlation
- suppression.

Some of them could be managed by Flume, eg. simple modification of a Flume Event fields.

The more advanced processing requires a dedicated software. [Apache Spark](https://spark.apache.org/), "a fast and general-purpose engine for large-scale data processing" was selected for this role.

Spark is horizontal scalable and run on a cluster. Spark can run standalone or over several existing cluster managers.
Apache Mesos adds High Availability to Apache Spark, as it resubmits a job in case of an error.
Spark revolves around the concept of a resilient distributed dataset (RDD), which is a fault-tolerant collection of elements that can be operated on in parallel.
Spark is able to execute both batch and streaming jobs. The selected elaboration tasks are performed using a streaming job, but with a similar code batch jobs could be executed on data stored in a long term archive. Spark provides the execution of streaming jobs by splitting the input data stream in batches of input data (RDD) which are processed using the batch functions. These batches of input data have a fixed time size. Spark allows to use [Map-Reduce](https://en.wikipedia.org/wiki/MapReduce) programming model making the processing of large input data rate with a parallel and distributed algorithms on a cluster. The Map functions fulfill the enrichment task, since acts event per event, whereas Reduce functions fulfill the aggregation task, since operate on all data belonging to the same RDD. Suppression and correlation tasks needed store old data in order to compute algorithms on them. Apache Spark allows to store data in memory, beyond in a no-volatile memory, like distributed storage. Since in this use case the processing the new data as quickly as possible is the priority, the in-memory processing is the best solution.
The Spark applications are written in Java and Scala, even if Scala is preferred since is less verbose and more intuitive.

To implement the aggregation task, the `reduceByKeyAndWindow` function is used: it allows to aggregate Flume event received in the same time window and with the same `key` using a specific function. The *key* field depending on which variables is desired aggregate e.g. on time or on time-and-hosts.

### 3.4 Storage
The storage component has the goal to store all the metric the Historical dashboard(s) need.

[InfluxDB](https://docs.influxdata.com/influxdb/v1.5/) is a "custom high-performance data store written specifically for time series data. It allows for high throughput ingest, compression and real-time querying of that same data". Its features make it the best solution to accomplish the database requirements.
InfluxDB allows to handle hundreds of data points per second: keeping the high precision raw data for only a limited time, and storing the lower precision. InfluxDB offers two features, Continuous Queries and Retention Policies, that help automate the process of downsampling data and expiring old data. InfluxDB provides InfluxQL as a SQL-like query language for interacting with stored data and low disk occupancy per measurement, three bytes for non-string values.

#### 3.4.1 Data organization
Timeseries data is stored in *measurements*, associable to the relation database tables. A database contains multiple measurements and multiple retention policies. Since a measurement could have the same name in multiple retention policies, an uniquely way to define it is: `<database_name>.<retention_policy_name>.<measurement_name>`. For example `collectd.ret_pol_1day.disk_read`

#### 3.4.2 Retention Policies and Continuous Queries
Adopt properly retention policies and continuous queries allow to minimize the disk usage and the computation requirements. Using retention policies, expire times can be associated to database. This feature provide a way to store both high time resolution data for a short period and low time resolution data for very long period. The downsampling processing could be done using continuous queries that read the source metric and compute the aggregation task to evaluate low time resolution time series. InfluxDB provides an extended set of aggregation function (mean, median, min, max, ...).

### 3.5 Dashboards
Historical and Near-real-time dashboards have the goal to plot time-series monitoring data using graphical objects on a interface accessible from the web.
[Grafana](https://grafana.com) has been chosen as data visualization tool since it offers customized historical record dashboards and real-time version is already foreseen in the road map. It can also generate alarms based on values coming from the database and thus not provide real-time alarms.
Grafana is able to retrieve data from multiple data sources like InfluxDB, Prometeus, ElasticSearch, Graphite and others, to provide an unique interface to visualize data coming from different back-ends.
Grafana supports multiple organizations in order to support a wide variety of deployment models, including using a single Grafana instance to provide service to multiple potentially untrusted Organizations.
Multiple organizations are configured to allow to multiple teams to share the same Grafana instance and work in an isolate environment.
A User is a named account in Grafana. A user can belong to one or more Organizations, and can be assigned different levels of privileges through roles.
A user’s organization membership is tied to a role that defines what the user is allowed to do in that organization. The roles are:
- Admin: add/edit data sources, add/edit organization users and teams, configure plugins and set organization settings.
- Editor: create/edit dashboards and alert rules
- Viewer: View any dashboard
All these roles will be configured to manage all the ALICE users.

Grafana supports a wide variety of internal and external ways for users to authenticate themselves and the CERN SSO it's used to allow CERN people to access to the dashboard, with a specific role.
A dashboard is logically divided in *rows* used to group *panels* together. Panel is the basic visualization building block. Currently five panel types are available:

- Graph
- Singlestat
- Dashlist
- Table
- Text

The Query Editor exposes capabilities of data source and allows to query metrics in a simple and graphical way. After having build a dashboard, the interface allows easily to setup the time window within plot the data and the refresh time.

The Historical dashboard has the goal to plot historical trends of significant metrics in order to valuate the functionality of all part in a wide time windows. The data to plot are downsampled historical data stored in the historical database (InfluxDB). For this purpose, the refresh time greater 10 seconds are considered adequate. Whereas the Near-real-time dashboard must plot data with the lowest latency, in any case below 1 second. To fulfill this requirement the data can't be retrieve so often from a database, a web-socket approach has been chosen. Grafana is planning to provide this feature in the next version.

### 3.6 Alarming
The alarming component has the goal to forward externally important alarms to experts. [Riemann](http://riemann.io/) has been selected since its capacity to inspect metrics on the fly and generate notifications when undesired behavior is detected. Riemann is able to execute simple processing on incoming data, like aggregation or filtering, and could support the streaming processing unit if needed. Its main task is the forwarding of alarms to experts. Riemann uses Clojure as programming language and in order to process events and send alerts and metrics a [Clojure](https://clojure.org/) code is needed. Clojure is a "dynamic, general-purpose programming language, combining the approachability and interactive development of a scripting language with an efficient and robust infrastructure for multithreaded programming". According the architecture shown in Figure 4, the Riemann instance receives data from a Flume custom Riemann Sink. HTTP, TCP and websocket protocol could be used to transmit events. Tests will established the best protocol. As alerts, email and Slack messages are been selected. Due to Riemann does not support cluster deployments, only simple and low cpu-usage processing tasks could be executed.

## 4. Deployment
In order to quickly and flawlessly deploy the monitoring tools [Ansible roles](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) were prepared for the following components:
- Flume
- Spark
- CollectD
- InfluxDB
- Grafana


 ### Team
 - [jvino](https://github.com/jvino) - Gioacchino Vino
 - [awegrzyn](https://github.com/awegrzyn) - Adam Wegrzynek
 - ? (reviewer)
