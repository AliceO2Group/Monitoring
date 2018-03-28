# InfluxDB installation and configuration

+ Add `influxdb` repo **(as root)**
~~~
cat > /etc/yum.repos.d/influxdb.repo <<EOF
[influxdb]
name = InfluxDB Repository - RHEL \$releasever
baseurl = https://repos.influxdata.com/rhel/\$releasever/\$basearch/stable
enabled = 1
gpgcheck = 1
gpgkey = https://repos.influxdata.com/influxdb.key
EOF
~~~

+ Install InfluxDB package **(as root)**
~~~
yum -y install influxdb collectd
~~~

+ Add UDP endpoint for application related metrics by editing configuration file `/etc/influxdb/influxdb.conf` with database name `test` and UDP port number `8088`. **(as root)**
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

+ Add an endpoint for `collectd` **(as root)**
~~~
[[collectd]]
  enabled = true
  bind-address = ":25826"
  database = "system-monitoring"
  typesdb = "/usr/share/collectd/types.db"
~~~

+ Open UDP port `25826` and `8088` **(as root)**
~~~
firewall-cmd --zone=public --permanent --add-port=8088/udp
firewall-cmd --zone=public --permanent --add-port=25826/udp
firewall-cmd --reload
~~~

+ Start InfluxDB **(as root)**
~~~
systemctl start influxdb
~~~

+ Create database `test` and `system-monitoring`
~~~
influx
influx> create database test
influx> create database system-monitoring
~~~
More details available at [InfluxDB page](https://docs.influxdata.com/influxdb/v1.2/introduction/installation/).
