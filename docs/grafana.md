# Grafana installation and configuration

+ Add Grafana repo **(as root)**
~~~
curl -s https://packagecloud.io/install/repositories/grafana/stable/script.rpm.sh | bash
~~~

+ Install Grafana package **(as root)**
~~~
yum -y install grafana
~~~

+ Open port 3000 **(as root)**
~~~
firewall-cmd --zone=public --add-port 3000/tcp --permanent
firewall-cmd --reload
~~~

+ Change default `admin_user` and `admin_password`: `/etc/grafana/grafana.ini`. **(as root)**

See more regarding configuration file in the official documentation: http://docs.grafana.org/installation/configuration/

+ (Enable SSL)
  + Set protocol to `https`, `ssl_mode` to `skip-verify` in configuration file
  + Generate private key and certificate via [CERN Certification Authority](https://ca.cern.ch/ca/host/HostCertificates.aspx)
  + Set `cert_file` and `cert_key` value in configuration file

+ (Configure LDAP-based login: `/etc/grafana/ldap.toml`)
See official documentation at the Grafana webpage: http://docs.grafana.org/installation/ldap/

+ Start Grafana **(as root)**
~~~
systemctl start grafana-server
~~~
