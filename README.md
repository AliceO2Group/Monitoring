# Monitoring
Monitoring module allows to inject user defined metrics and monitor the process itself. It supports multiple backends, protocols and data formats.

## Table of contents
1. [Installation](#installation)
  * [RPM (CentOS 7 only)](#rpm-centos-7-only)
  * [aliBuild](#alibuild)
  * [Manual](#manual)
2. [Getting started](#getting-started)
3. [Code snippets](#code-snippets)
4. [System monitoring and server-side backends installation and configuration](#system-monitoring-server-side-backends-installation-and-configuration)
  * [collectD](#collectd)
  * [MonALISA Service](#monalisa-service)
  * [Flume](#flume)
  * [Grafana](#grafana)
  * [Zabbix](#zabbix)

## Installation
### RPM (CentOS 7 only)
+ Install `CERN-CA-certs` package (required by `alisw` repo)
~~~
sudo yum -y install CERN-CA-certs
~~~

+ Add `alisw` repo
~~~
su -c 'cat > /etc/yum.repos.d/alisw-el7.repo <<EOF
[alisw-el7]
name=ALICE Software - EL7
baseurl=https://ali-ci.cern.ch/repo/RPMS/el7.x86_64/
enabled=1
gpgcheck=0
EOF'
~~~

+ Install Monitoring RPM package
~~~
sudo yum -y install alisw-Monitoring+v1.3.0-1.x86_64
~~~

+ Configure Modules
~~~
export MODULEPATH=/opt/alisw/el7/modulefiles:$MODULEPATH
~~~

+ Load enviroment
~~~
eval `modulecmd bash load Monitoring/v1.3.0-1`
~~~
The installation directory is: `/opt/alisw/el7/Monitoring/v1.3.0-1`

### aliBuild
+ Install packages required by aliBuild
~~~
sudo yum -y install python-pip git gcc-c++ bison flex bzip2-devel ncurses-devel
~~~

+ Install [aliBuild](https://alisw.github.io/alibuild/) via `pip`
~~~
sudo pip install alibuild
~~~

+ Then follow instructions below
~~~
aliBuild init Monitoring@master
aliDoctor Monitoring
aliBuild --defaults o2 build Monitoring
alienv load Monitoring/latest
~~~

### Manual
Manual installation of the O<sup>2</sup> Monitoring module.

#### Requirements
+ C++ compiler with C++14 support, eg.:
  + `gcc-c++` package from `devtoolset-6` on CentOS 7
  + `clang++` on Mac OS
+ Boost >= 1.56
+ libcurl
+ [ApMon](http://monalisa.caltech.edu/monalisa__Download__ApMon.html) (optional)
+ [Configuration module](https://github.com/AliceO2Group/Configuration#manual-installation)

#### Monitoring module compilation
~~~
git clone https://github.com/AliceO2Group/Monitoring.git
cd Monitoring; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installdir>
make -j
make install
~~~

## Getting started
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

### Creating monitoring instance
1. The recommended way of getting (reference to) monitoring instance is *MonitoringFactory*.
Before using the factory *Configure* method must be called providing URI to configuration file or central backend. This method should be called only once per process (following calls will not have any effect).
```cpp
AliceO2::Monitoring::MonitoringFactory::Configure("file://../Monitoring/examples/config-default.ini");
AliceO2::Monitoring::MonitoringFactory::Get();
```

2. Second way creates dedicated monitoring instance. It's only recommended to use when different configuration URI is needed within the same process.
```cpp
Monitoring::Create("file://../Monitoring/examples/config-default.ini");
```

### Sending a metric
Metric can be sent by one of the following ways:
1. By creating and moving metric object:
   + `send(Metric&& metric)`

   In addition, two additional methods can be chained:
   + `addTags(std::vector<Tag>&& tags)`
   + `setTimestamp(std::chrono::time_point<std::chrono::system_clock>& timestamp)`

2. By providing metric parameters:
   + `send(T value, std::string name)`
   + `sendTagged(T value, std::string name, std::vector<Tag>&& tags)`
   + `sendTimed(T value, std::string name, std::chrono::time_point<std::chrono::system_clock>& timestamp)`

## Derived metrics
The module can calculate derived metrics. To do so, use `addDerivedMetric(std::string name, DerivedMetricMode mode)` with one of two available modes:
+ `DerivedMetricMode::RATE` - rate between two following metrics;
+ `DerivedMetricMode::AVERAGE` - average value of all metrics stored in cache;

Derived metrics are generated each time as new value is passed to the module. Their names are suffixed with derived mode name.

### Processes monitoring
To enable process monitoring *ProcessMonitor.enable* flag in configuration file must be set to 1 - see [Configuration file](#configuration-file) section. The following metrics are generated every N seconds (N can be specified in the config - *ProcessMonitor.interval*):
+ **etime** - elapsed time since the process was started, in the form [[DD-]hh:]mm:ss
+ **pcpu** - cpu utilization of the process in "##.#" format. Currently, it is the CPU time used divided by the time the process has been running (cputime/realtime ratio), expressed as a percentage.  It will not add up to 100% unless you are lucky
+ **pmem** - ratio of the process's resident set size  to the physical memory on the machine, expressed as a percentage
+ **bytesReceived** - the total number of bytes of data received by the process (per interface)
+ **bytesTransmitted** - the total number of bytes of data transmitted by the process (per interface).

### Monitoring backends
Metrics are pushed to one or multiple backends. The module currently supports three backends - see table below. Enabling/Disabling backends is done via configuration file.

| Backend name     | Description                    | Transport                    | Client-side requirements   | Handling tags  |
| ---------------- |:------------------------------:|:----------------------------:|:--------------------------:| --------------:|
| InfluxDB         | InfluxDB time series database  | HTTP / UDP (InfluxDB Line Protocol) | cURL / boost asio   | Supported by InfluxDB Line Protocol |
| ApMonBackend     | MonALISA Serivce               | UDP                          | ApMon                      | Default tags concatenated with entity; Metric tags concatenated with name |
| InfoLoggerBackned| Temporary replaced by internal logging              | -                            | (as log message)           | Added to the end of message |
| Flume            | Collects, aggragate monitoring data | UDP (JSON)              | boost asio                 | In Flume Event header |
| Zabbix           | Via Zabbix trapper item        | TCP (Zabbix protocol)        | boost asio                 | Not supported |

### Configuration file
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
+ Zabbix
  + enable - enable Zabbix backend
  + port
  + hostname
+ ProcessMonitor
  + enable - enable process monitor
  + interval - updates interval[]
+ DerivedMetrics
  + maxCacheSize - maximum size of vector

See sample in [examples/config-default.ini](https://github.com/awegrzyn/Monitoring/blob/master/examples/config-default.ini).

## Code snippets
Code snippets are available in [example](https://github.com/awegrzyn/Monitoring/tree/master/examples) directory.

### Sending user defined metric - examples/1-Basic.cxx
```cpp
// configure monitoring (once per process), pass configuration path as parameter
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

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
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

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
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

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
Monitoring::Configure("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

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
auto collector = Monitoring::Create("file:///home/awegrzyn/hackathon/Monitoring/examples/config-default.ini");

// now send an application specific metric
// 10 is the value
// myMetric is the name of the metric
//  
// 1. by copying values
collector->send(10, "myMetric");
  
// 2. by creating and moving metric object
collector->send({10, "myMetric"});
```


## System monitoring, server-side backends installation and configuration
This guide explains manual installation. For `ansible` deployment see [AliceO2Group/system-configuration](https://gitlab.cern.ch/AliceO2Group/system-configuration/tree/master/ansible) gitlab repo.

### collectD
+ Install collectd package
~~~
sudo yum -y install collectd
~~~

+ Edit configuration file: `/etc/collectd.conf`
~~~
Interval     10
Include "/etc/collectd.d"
~~~

+ Configure `network` write plugin: `/etc/collectd.d/network.conf` in order to push metrics to InfluxDB instance. Replace `<influxdb-host>` with InfluxDB hostname.
~~~
LoadPlugin network
<Plugin network>
  Server "<influxdb-host>" "25826"
</Plugin>
~~~

+ Configure `cpu` module: `/etc/collectd.d/cpu.conf`
~~~
LoadPlugin cpu
<Plugin cpu>
  ReportByCpu true
  ReportByState true
  ValuesPercentage true
</Plugin>
~~~

+ Configure `disk` plugin: `/etc/collectd.d/disk.conf`
~~~
LoadPlugin disk
<Plugin disk>
  Disk "/[hs]d[a-f][0-9]?$/"
  IgnoreSelected false
  UseBSDName false
  UdevNameAttr "DEVNAME"
</Plugin>
~~~

+ Configure `interface` plugin: `/etc/collectd.d/interface.conf`
~~~
LoadPlugin interface
~~~

+ Configure `load` plugin: `/etc/collectd.d/load.conf`
~~~
LoadPlugin interface
~~~

+ Configure `memory` plugin: `/etc/collectd.d/memory.conf`
~~~
LoadPlugin memory
~~~

+ Configure `uptime` plugin: `/etc/collectd.d/uptime.conf`
~~~
LoadPlugin uptime
~~~

+ Start collectd
~~~
sudo systemctl start collectd.service
sudo systemctl enable collectd.service
~~~

### InfluxDB
+ Add `influxdb` repo
~~~
su -c 'cat > /etc/yum.repos.d/influxdb.repo <<EOF
[influxdb]
name = InfluxDB Repository - RHEL \$releasever
baseurl = https://repos.influxdata.com/rhel/\$releasever/\$basearch/stable
enabled = 1
gpgcheck = 1
gpgkey = https://repos.influxdata.com/influxdb.key
EOF'
~~~

+ Install InfluxDB package
~~~
sudo yum -y install influxdb collectd
~~~

+ Add UDP endpoint for application related metrics by editing configuration file `/etc/influxdb/influxdb.conf` with database name `test` and UDP port number `8088`.
~~~
[[udp]]
  enabled = true
  bind-address = ":8088"
  database = "test"
  batch-size = 5000
  batch-timeout = "1s"
  batch-pending = 100
  read-buffer = 8388608
~~~

+ Add an endpoint for `collectd`
~~~
[[collectd]]
  enabled = true
  bind-address = ":25826"
  database = "system-monitoring"
  typesdb = "/usr/share/collectd/types.db"
~~~

+ Open UDP port `25826` and `8088`
~~~
sudo firewall-cmd --zone=public --permanent --add-port=8088/udp
sudo firewall-cmd --zone=public --permanent --add-port=25826/udp
sudo firewall-cmd --reload
~~~

+ Start InfluxDB
~~~
sudo systemctl start influxdb
~~~

+ Create database `test` and `system-monitoring`
~~~
influx
influx> create database test
influx> create database system-monitoring
~~~
More details available at [InfluxDB page](https://docs.influxdata.com/influxdb/v1.2/introduction/installation/).

### Flume
+ Install Java
~~~
sudo yum -y install java
~~~

+ Download [latest release](http://www-eu.apache.org/dist/flume/1.7.0/apache-flume-1.7.0-bin.tar.gz) of Apache Flume

+ Unpack file
~~~
tar -xvzf apache-flume-1.7.0-bin.tar.gz
~~~

+ Install custom source and/or sink from [MonitoringCustomComponents repo]( https://github.com/AliceO2Group/MonitoringCustomComponents).
Adjust configuration file according to source/sink instructions. The sample configuration file is available in `conf/flume-conf.properties.template`.

+ Launch Flume using following command:
~~~
$ bin/flume-ng agent -n <agent-name> -c conf -f conf/<flume-confing>
~~~
Set correct `<agent-name>` and `<flume-confing>` name.

See [Flume User Guide](https://flume.apache.org/FlumeUserGuide.html) documentation for more details.

### Grafana

+ Add Grafana repo
~~~
curl -s https://packagecloud.io/install/repositories/grafana/stable/script.rpm.sh | sudo bash
~~~

+ Install Grafana package
~~~
sudo yum -y install grafana
~~~

+ Open port 3000
~~~
sudo firewall-cmd --zone=public --add-port 3000/tcp --permanent
sudo firewall-cmd --reload
~~~

+ Change default `admin_user` and `admin_password`: `/etc/grafana/grafana.ini`.
See more regarding configuration file in the official documentation: http://docs.grafana.org/installation/configuration/

+ (Enable SSL)
  + Set protocol to `https`, `ssl_mode` to `skip-verify` in configuration file
  + Generate private key and certificate via [CERN Certification Authority](https://ca.cern.ch/ca/host/HostCertificates.aspx)
  + Set `cert_file` and `cert_key` value in configuration file

+ (Configure LDAP-based login: `/etc/grafana/ldap.toml`)
See official documentation at the Grafana webpage: http://docs.grafana.org/installation/ldap/

+ Start Grafana
~~~
sudo systemctl start grafana-server
~~~

### MonALISA Service
Follow official [MonALISA Service Installation Guide](http://monalisa.caltech.edu/monalisa__Documentation__Service_Installation_Guide.html).

### Zabbix
#### Installation
+ Add Zabbix repository
~~~
sudo rpm -ivh https://repo.zabbix.com/zabbix/3.2/rhel/7/x86_64/zabbix-release-3.2-1.el7.noarch.rpm
~~~

+ Install Zabbix packages
~~~
sudo yum -y install zabbix-server-mysql zabbix-web-mysql mariadb-server
~~~

+ Start MariaDB
~~~
sudo systemctl enable mariadb
sudo systemctl start mariadb
~~~

+ Configure MariaDB via wizard
Set database `<root_password>`.
~~~
mysql_secure_installation
~~~

+ Create zabbix user and database
Replace `<root_password>` with root database password.
Replace `<password>` with password you want to set to zabbix database account.
~~~
$ mysql -uroot -p<root_password>
create database zabbix character set utf8 collate utf8_bin;
grant all privileges on zabbix.* to zabbix@localhost identified by '<password>';
~~~

+ Import initial database schema
Find out the version of `zabbix-server-mysql` package by running `rpm -q zabbix-server-mysql`.
~~~
gunzip /usr/share/doc/zabbix-server-mysql-3.2.6/create.sql.gz
cat /usr/share/doc/zabbix-server-mysql-3.2.6/create.sql | mysql -uzabbix -p zabbix
~~~

+ Set database details in `/etc/zabbix/zabbix_server.conf` file
~~~
DBHost=localhost
DBName=zabbix
DBUser=zabbix
DBPassword=<password>
~~~

+ Edit SELinux policy
~~~
sudo setsebool -P httpd_can_connect_zabbix on
~~~

+ Open HTTP(s) and Zabbix related ports
~~~
sudo firewall-cmd --zone=public --add-port 10051/tcp --permanentt
sudo firewall-cmd --add-service=http --zone=public --permanent
sudo firewall-cmd --add-service=https --zone=public --permanent
sudo firewall-cmd --reload
~~~

+ Start Zabbix server
~~~
sudo systemctl start zabbix-server
sudo systemctl enable zabbix-server
~~~

#### Host and metric configuration
+ Open Zabbix web interface
+ Go to `Configuration` -> `Hosts`
+ Go to `Create a new host` to add a host that is allowed to push metrics. Use `Linux servers` as group. Agent interface can be set as `127.0.0.1:10050`.
+ Go to `Items` tab to create a metric that will be accepted by Zabbix. Set `Type` to `Zabbix trapper`. Set proper `Type information`.
