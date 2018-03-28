# Collectd - system monitoring
Project website: https://collectd.org

## Installation

+ Install collectd package **(as root)**
~~~
yum -y install collectd
~~~

## Configuration

+ Edit configuration file: `/etc/collectd.conf`**(as root)**
~~~
Interval     10  
Include "/etc/collectd.d"
~~~

+ Configure `network` write plugin: `/etc/collectd.d/network.conf` in order to push metrics to InfluxDB instance. Replace `<influxdb-host>` with InfluxDB hostname. **(as root)**
~~~
LoadPlugin network
<Plugin network>
  Server "<influxdb-host>" "25826"
</Plugin>
~~~

+ Configure `cpu` module: `/etc/collectd.d/cpu.conf` **(as root)**
~~~
LoadPlugin cpu 
<Plugin cpu>
  ReportByCpu true
  ReportByState true
  ValuesPercentage true
</Plugin>
~~~

+ Configure `disk` plugin: `/etc/collectd.d/disk.conf` **(as root)**
~~~
LoadPlugin disk
<Plugin disk>
  Disk "/[hs]d[a-f][0-9]?$/"
  IgnoreSelected false
  UseBSDName false
  UdevNameAttr "DEVNAME"
</Plugin>
~~~

+ Configure `interface` plugin: `/etc/collectd.d/interface.conf` **(as root)**
~~~
LoadPlugin interface
~~~

+ Configure `load` plugin: `/etc/collectd.d/load.conf` **(as root)**
~~~
LoadPlugin interface
~~~

+ Configure `memory` plugin: `/etc/collectd.d/memory.conf` **(as root)**
~~~
LoadPlugin memory
~~~

+ Configure `uptime` plugin: `/etc/collectd.d/uptime.conf` **(as root)**
~~~
LoadPlugin uptime
~~~

+ Start collectd **(as root)**
~~~
systemctl start collectd.service
systemctl enable collectd.service
~~~
