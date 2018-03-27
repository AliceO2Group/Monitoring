# namespace `monitoring`

ALICE O2 [Monitoring](#classo2_1_1monitoring_1_1Monitoring) system.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`namespace `[``backends``](#namespaceo2_1_1monitoring_1_1backends)    | [Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.
`namespace `[``transports``](#namespaceo2_1_1monitoring_1_1transports)    | [Monitoring](#classo2_1_1monitoring_1_1Monitoring) transports.
`class `[``Backend``](#classo2_1_1monitoring_1_1Backend)    | [Backend](#classo2_1_1monitoring_1_1Backend) pure virtual interface.
`class `[``DerivedMetrics``](#classo2_1_1monitoring_1_1DerivedMetrics)    | Calculates derived metrics.
`class `[``Metric``](#classo2_1_1monitoring_1_1Metric)    | Represents metric parameters except (value, name, entity and timestamp)
`class `[``Monitoring``](#classo2_1_1monitoring_1_1Monitoring)    | Collects metrics from user and dispatches them to selected [Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.
`class `[``MonitoringException``](#classo2_1_1monitoring_1_1MonitoringException)    |
`class `[``MonitoringFactory``](#classo2_1_1monitoring_1_1MonitoringFactory)    |
`class `[``MonitoringInternalException``](#classo2_1_1monitoring_1_1MonitoringInternalException)    |
`class `[``MonLogger``](#classo2_1_1monitoring_1_1MonLogger)    | Simple [Monitoring](#classo2_1_1monitoring_1_1Monitoring) logging class.
`class `[``ProcessDetails``](#classo2_1_1monitoring_1_1ProcessDetails)    |
`class `[``ProcessMonitor``](#classo2_1_1monitoring_1_1ProcessMonitor)    | Monitors current process and/or other processes running at the same machien.
`class `[``Tag``](#classo2_1_1monitoring_1_1Tag)    | Represent key-value metric tag.
`class `[``VariantVisitorAdd``](#classo2_1_1monitoring_1_1VariantVisitorAdd)    |
`class `[``VariantVisitorRate``](#classo2_1_1monitoring_1_1VariantVisitorRate)    |
`class `[``Version``](#classo2_1_1monitoring_1_1Version)    | Information about the current [Monitoring](#classo2_1_1monitoring_1_1Monitoring) version.

# namespace `backends`

[Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``ApMonBackend``](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend)    | [Backend](#classo2_1_1monitoring_1_1Backend) that uses AppMon (MonALISA)
`class `[``Flume``](#classo2_1_1monitoring_1_1backends_1_1Flume)    | [Backend](#classo2_1_1monitoring_1_1Backend) that sends metrics to custum Apache [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) source.
`class `[``InfluxDB``](#classo2_1_1monitoring_1_1backends_1_1InfluxDB)    | [Backend](#classo2_1_1monitoring_1_1Backend) that sends metrics to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) time-series databse.
`class `[``InfoLoggerBackend``](#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend)    | [Backend](#classo2_1_1monitoring_1_1Backend) that injects metrics to InfoLogger.

# class `ApMonBackend`

```
class ApMonBackend
  : public o2::monitoring::Backend
```  

[Backend](#classo2_1_1monitoring_1_1Backend) that uses AppMon (MonALISA)

Uses [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend) library to push metric to MonALISA Service. [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend) accepts timestamps as integers, therefore cast is needed (see method [convertTimestamp](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1af91f476965c945f31cd6ffd41dcfa6ef))

Adam Wegrzynek [adam.wegrzynek@cern.ch](mailto:adam.wegrzynek@cern.ch)

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  ApMonBackend(const std::string & path)` | Constructs AppMon backend.
`public  ~ApMonBackend() = default` | Default destructor.
`public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metrics not related to each other @param metrics vector of metrics.
`public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Sends metric via MonALISA [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend)'s intances is type-aware therefore cast of metric value is needed.
`public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends grouped metrics under common measuremet name.
`public virtual void addGlobalTag(std::string name,std::string value)` | Extends entity value.

## Members

### `public  ApMonBackend(const std::string & path)`

Constructs AppMon backend.

#### Parameters
* `configurationFile` filepath to [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend) configuration file

### `public  ~ApMonBackend() = default`

Default destructor.



### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Sends metric via MonALISA [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend)'s intances is type-aware therefore cast of metric value is needed.

#### Parameters
* `metric` reference to metric object:

### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends grouped metrics under common measuremet name.

#### Parameters
* `name` measurement name


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)`

Extends entity value.

#### Parameters
* `name` tag name (unused)


* `value` tag value that is concatenated to entity string


# class `Flume`

```
class Flume
  : public o2::monitoring::Backend
```  

[Backend](#classo2_1_1monitoring_1_1Backend) that sends metrics to custum Apache [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) source.

Metrics are serialized to JSON strings and send via UDP

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  Flume(const std::string & host,unsigned int port)` | Constructor, uses UDP transport.
`public  ~Flume() = default` | Default destructor.
`public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)` | Convert timestamp to unsigned long as required by [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume).
`public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.
`public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metrics not related to each other @param metrics vector of metrics.
`public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metric in single packet Not supported by the backend therefore it falls back to sending metric one by one TODO: changed required in [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) Source.
`public virtual void addGlobalTag(std::string name,std::string value)` | Adds tag.

## Members

### `public  Flume(const std::string & host,unsigned int port)`

Constructor, uses UDP transport.

#### Parameters
* `hostname` [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) HTTP endpoint hostanme


* `port` [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) HTTP endpoint port number

### `public  ~Flume() = default`

Default destructor.



### `public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)`

Convert timestamp to unsigned long as required by [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume).

#### Parameters
* `chrono` time_point timestamp





#### Returns
timestamp in nanoseconds

### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metric in single packet Not supported by the backend therefore it falls back to sending metric one by one TODO: changed required in [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) Source.

#### Parameters
* `name` measurement name


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)`

Adds tag.

#### Parameters
* `name` tag name


* `value` tag value


# class `InfluxDB`

```
class InfluxDB
  : public o2::monitoring::Backend
```  

[Backend](#classo2_1_1monitoring_1_1Backend) that sends metrics to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) time-series databse.

Metrics are converted into Influx Line protocol and then sent via one of available transports

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  InfluxDB(const std::string & host,unsigned int port)` | Constructor for both HTTP and UDP transports.
`public  InfluxDB(const std::string & host,unsigned int port,const std::string & path)` | Constructor for both HTTP and UDP transports.
`public  ~InfluxDB() = default` | Default destructor.
`public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)` | Convert timestamp to unsigned long as required by [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB).
`public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.
`public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metrics not related to each other @param metrics vector of metrics.
`public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple values in single measurement.
`public virtual void addGlobalTag(std::string name,std::string value)` | Adds tag.

## Members

### `public  InfluxDB(const std::string & host,unsigned int port)`

Constructor for both HTTP and UDP transports.

#### Parameters
* `hostname` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint hostname


* `port` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint port number

### `public  InfluxDB(const std::string & host,unsigned int port,const std::string & path)`

Constructor for both HTTP and UDP transports.

#### Parameters
* `hostname` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint hostname


* `port` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint port number


* `path` Query path

### `public  ~InfluxDB() = default`

Default destructor.



### `public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)`

Convert timestamp to unsigned long as required by [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB).

#### Parameters
* `chrono` time_point timestamp





#### Returns
timestamp in nanoseconds

### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple values in single measurement.

#### Parameters
* `name` measurement name


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)`

Adds tag.

#### Parameters
* `name` tag name


* `value` tag value


# class `InfoLoggerBackend`

```
class InfoLoggerBackend
  : public o2::monitoring::Backend
```  

[Backend](#classo2_1_1monitoring_1_1Backend) that injects metrics to InfoLogger.

InfoLogger does not support std::chrono::time_point therefore timestamps is converted to unsigned long (see [convertTimestamp](#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1a100732ee85d6f0a29cd4fe10a85753b5))

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  InfoLoggerBackend()` | Default constructor.
`public  ~InfoLoggerBackend() = default` | Default destructor.
`public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Sends metric to InfoLogger library.
`public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metrics not related to each other @param metrics vector of metrics.
`public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metric at once Not supported by the backend therefore it falls back to sending metric one by one.
`public virtual void addGlobalTag(std::string name,std::string value)` | Adds tag.

## Members

### `public  InfoLoggerBackend()`

Default constructor.



### `public  ~InfoLoggerBackend() = default`

Default destructor.



### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Sends metric to InfoLogger library.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metric at once Not supported by the backend therefore it falls back to sending metric one by one.

#### Parameters
* `name` measurement name


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)`

Adds tag.

#### Parameters
* `name` tag name


* `value` tag value


# namespace `transports`

[Monitoring](#classo2_1_1monitoring_1_1Monitoring) transports.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``HTTP``](#classo2_1_1monitoring_1_1transports_1_1HTTP)    | [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST transport for [Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.
`class `[``TCP``](#classo2_1_1monitoring_1_1transports_1_1TCP)    | Transport that sends string formatted metrics via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).
`class `[``TransportInterface``](#classo2_1_1monitoring_1_1transports_1_1TransportInterface)    | Transport interface for backends.
`class `[``UDP``](#classo2_1_1monitoring_1_1transports_1_1UDP)    | Transport that sends string formatted metrics via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

# class `HTTP`

```
class HTTP
  : public o2::monitoring::transports::TransportInterface
```  

[HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST transport for [Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.

Allows to push string formatted metrics as [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST requests via cURL

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  HTTP(const std::string & url)` | Constructor.
`public  ~HTTP() = default` | Default destructor.
`public virtual void send(std::string && post)` | Sends metric via [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST.

## Members

### `public  HTTP(const std::string & url)`

Constructor.

#### Parameters
* `url` URL of [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) server endpoint

### `public  ~HTTP() = default`

Default destructor.



### `public virtual void send(std::string && post)`

Sends metric via [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST.

#### Parameters
* `metric` r-value reference string formatted metric


# class `TCP`

```
class TCP
  : public o2::monitoring::transports::TransportInterface
```  

Transport that sends string formatted metrics via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  TCP(const std::string & hostname,int port)` | Constructor.
`public  ~TCP() = default` | Default destructor.
`public virtual void send(std::string && message)` | Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).
`public void read()` | Dummy read method - Forwards read out buffer to cout.

## Members

### `public  TCP(const std::string & hostname,int port)`

Constructor.

#### Parameters
* `hostname` InfluxDB instance hostname

### `public  ~TCP() = default`

Default destructor.



### `public virtual void send(std::string && message)`

Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

#### Parameters
* `lineMessage` r-value string formated

### `public void read()`

Dummy read method - Forwards read out buffer to cout.




# class `TransportInterface`


Transport interface for backends.

Represents transport protocol for backends that already formatted metrcis can be sent by Eg. [TCP](#classo2_1_1monitoring_1_1transports_1_1TCP), [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP) or [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP)

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  TransportInterface() = default` |
`public virtual  ~TransportInterface() = default` |
`public void send(std::string && message)` | Sends metric via given transport.

## Members

### `public  TransportInterface() = default`





### `public virtual  ~TransportInterface() = default`





### `public void send(std::string && message)`

Sends metric via given transport.

#### Parameters
* `message` r-value to string formatted metric


# class `UDP`

```
class UDP
  : public o2::monitoring::transports::TransportInterface
```  

Transport that sends string formatted metrics via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  UDP(const std::string & hostname,int port)` | Constructor.
`public  ~UDP() = default` | Default destructor.
`public virtual void send(std::string && message)` | Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

## Members

### `public  UDP(const std::string & hostname,int port)`

Constructor.

#### Parameters
* `hostname` InfluxDB instance hostname

### `public  ~UDP() = default`

Default destructor.



### `public virtual void send(std::string && message)`

Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

#### Parameters
* `lineMessage` r-value string formated


# class `Backend`


[Backend](#classo2_1_1monitoring_1_1Backend) pure virtual interface.

Interface that allows to send a metric to remote backend. In addition, default tagset (for all handled metrics) can be created.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  Backend() = default` | Default constructor.
`public virtual  ~Backend() = default` | Default destructor.
`public void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Sends metric via backend.
`public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple metrics not related to each other.
`public void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple related to each other metrics under a common name If not supported by backend, falls back into sending single metrics.
`public void addGlobalTag(std::string name,std::string value)` | Sets a tag.

## Members

### `public  Backend() = default`

Default constructor.



### `public virtual  ~Backend() = default`

Default destructor.



### `public void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Sends metric via backend.



### `public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple metrics not related to each other.



### `public void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple related to each other metrics under a common name If not supported by backend, falls back into sending single metrics.



### `public void addGlobalTag(std::string name,std::string value)`

Sets a tag.




# class `DerivedMetrics`


Calculates derived metrics.

Calculates derived metrics such as rate or average values... (available modes are enumerated in DerivedMetricMode class) For this reason past metrics are stored in a container (std::map). It applies only to metrics registered via #registerMetric method

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::unordered_map< std::string, `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > mStorage` | Cache of registered metrics (metric name / vector of metric pointers).
`public  DerivedMetrics() = default` | Default constructor.
`public  ~DerivedMetrics() = default` | Default destructor.
`public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` rate(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` | Calculates rate value based on metrics stored in mCache map.
`public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` increment(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` |

## Members

### `public std::unordered_map< std::string, `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > mStorage`

Cache of registered metrics (metric name / vector of metric pointers).



### `public  DerivedMetrics() = default`

Default constructor.



### `public  ~DerivedMetrics() = default`

Default destructor.



### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` rate(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`

Calculates rate value based on metrics stored in mCache map.

#### Parameters
* `name` metric name





#### Returns
metric with calculated rate value

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` increment(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)`






# class `Metric`


Represents metric parameters except (value, name, entity and timestamp)



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  Metric(int value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` | Initialize class variables.
`public  Metric(std::string value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` | Initialize class variables.
`public  Metric(double value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` | Initialize class variables.
`public  Metric(uint64_t value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` | Initialize class variables.
`public  Metric(boost::variant< int, std::string, double, uint64_t >,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` | Initialize class variables, required by derived metrics logic.
`public  ~Metric() = default` | Default destructor.
`public std::string getName() const` | Name getter.
`public void setName(std::string name)` | Name setter.
`public std::chrono::time_point< std::chrono::system_clock > getTimestamp() const` | Timestamp getter.
`public boost::variant< int, std::string, double, uint64_t > getValue() const` | Value getter.
`public int getType() const` | Value type getter.
`public std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > getTags() const` | [Tag](#classo2_1_1monitoring_1_1Tag) list getter.
`public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && setTimestamp(std::chrono::time_point< std::chrono::system_clock > & timestamp)` | Sets user defined timestamp.
`public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && addTags(std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > && tags)` | Add user defined tags.

## Members

### `public  Metric(int value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)`

Initialize class variables.

#### Parameters
* `value` metric value (int)


* `name` metric name


* `timestamp` metric timestamp in milliseconds

### `public  Metric(std::string value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)`

Initialize class variables.

#### Parameters
* `value` metric value (string)


* `name` the metric name


* `timestamp` metric timestamp in milliseconds

### `public  Metric(double value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)`

Initialize class variables.

#### Parameters
* `value` metric value (double)


* `name` metric name


* `timestamp` metric timestamp in milliseconds

### `public  Metric(uint64_t value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)`

Initialize class variables.

#### Parameters
* `value` metric value (uint64_t)


* `name` metric name


* `timestamp` metric timestamp in milliseconds

### `public  Metric(boost::variant< int, std::string, double, uint64_t >,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)`

Initialize class variables, required by derived metrics logic.

#### Parameters
* `value` metric value (boost variant)


* `name` metric name


* `timestamp` metric timestamp in milliseconds

### `public  ~Metric() = default`

Default destructor.



### `public std::string getName() const`

Name getter.

#### Returns
metric name

### `public void setName(std::string name)`

Name setter.

#### Parameters
* `new` name of the metric

### `public std::chrono::time_point< std::chrono::system_clock > getTimestamp() const`

Timestamp getter.

#### Returns
metric timestamp

### `public boost::variant< int, std::string, double, uint64_t > getValue() const`

Value getter.

#### Returns
metric value

### `public int getType() const`

Value type getter.

#### Returns
type of value stores inside metric : 0 - int, 1 - std::string, 2 - double

### `public std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > getTags() const`

[Tag](#classo2_1_1monitoring_1_1Tag) list getter.

#### Returns
vector of tags

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && setTimestamp(std::chrono::time_point< std::chrono::system_clock > & timestamp)`

Sets user defined timestamp.

#### Parameters
* `timestamp`





#### Returns
r-value to "this" - to be able to chain methods

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && addTags(std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > && tags)`

Add user defined tags.

#### Parameters
* `r-value` to vector of tags





#### Returns
r-value to "this" - to be able to chain methods


# class `Monitoring`


Collects metrics from user and dispatches them to selected [Monitoring](#classo2_1_1monitoring_1_1Monitoring) backends.

It also monitors the process itself.

Collects user-defined metrics (see [Metric](#classo2_1_1monitoring_1_1Metric) class) and pushes them through all selected backends (see [Backend](#classo2_1_1monitoring_1_1Backend)). Calculates derived metrics, such as rate and average value (see #addDerivedMetric method). Adds default tags to each metric: proces name, hostname. Monitors the process itself - including memory, cpu usage and running time (see [ProcessMonitor](#classo2_1_1monitoring_1_1ProcessMonitor)).

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` & operator=(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete` | Disables copy constructor.
`public  Monitoring(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete` |
`public  Monitoring()` | Initializes backends based on passed configuration.
`public void addBackend(std::unique_ptr< `[`Backend`](#classo2_1_1monitoring_1_1Backend)` > backend)` | Creates backend and appends it to backend list.
`public  ~Monitoring()` | Joins process monitor thread if possible.
`public void send(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` && metric,`[`DerivedMetricMode`](#namespaceo2_1_1monitoring_1afdc1126ed1982b9a501fb36c63110ab2)` mode)` | Sends a metric to all avaliabes backends If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)
`public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple not related to each other metrics.
`public void sendGrouped(std::string name,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` | Sends multiple realated to each other metrics under a common measurement name You can imagine it as a data point with multiple values If it's not supported by a backend it fallbacks into sending one by one.
`public void enableProcessMonitoring(const unsigned int interval)` | Enables process monitoring.
`public void startTimer(std::string name)` | Starts timing Sets a start timestamp and timeout.
`public void stopAndSendTimer(std::string name)` | Stops timing Sets stop timestamp, calculates delta and sends value.
`public void flushBuffer()` | Flushes metric buffer (this can also happen when buffer is full)
`public void enableBuffering(const unsigned int size)` | Enables metric buffering.

## Members

### `public `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` & operator=(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete`

Disables copy constructor.



### `public  Monitoring(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete`





### `public  Monitoring()`

Initializes backends based on passed configuration.

Instantiates derived metrics processor (see [DerivedMetrics](#classo2_1_1monitoring_1_1DerivedMetrics) class) and process monitor (see [ProcessMonitor](#classo2_1_1monitoring_1_1ProcessMonitor)).

### `public void addBackend(std::unique_ptr< `[`Backend`](#classo2_1_1monitoring_1_1Backend)` > backend)`

Creates backend and appends it to backend list.



### `public  ~Monitoring()`

Joins process monitor thread if possible.



### `public void send(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` && metric,`[`DerivedMetricMode`](#namespaceo2_1_1monitoring_1afdc1126ed1982b9a501fb36c63110ab2)` mode)`

Sends a metric to all avaliabes backends If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)

#### Parameters
* `metric` r-value to metric object

### `public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple not related to each other metrics.

#### Parameters
* `metrics` vector of metrics

### `public void sendGrouped(std::string name,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)`

Sends multiple realated to each other metrics under a common measurement name You can imagine it as a data point with multiple values If it's not supported by a backend it fallbacks into sending one by one.

#### Parameters
* `name` measurement name


* `metrics` list of metrics

### `public void enableProcessMonitoring(const unsigned int interval)`

Enables process monitoring.

#### Parameters
* `interval` refresh interval

### `public void startTimer(std::string name)`

Starts timing Sets a start timestamp and timeout.

#### Parameters
* `name` metric name


* `timeout` timeout

### `public void stopAndSendTimer(std::string name)`

Stops timing Sets stop timestamp, calculates delta and sends value.

#### Parameters
* `name` metric name

### `public void flushBuffer()`

Flushes metric buffer (this can also happen when buffer is full)



### `public void enableBuffering(const unsigned int size)`

Enables metric buffering.

#### Parameters
* `size` buffer size


# class `MonitoringException`

```
class MonitoringException
  : public std::exception
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  MonitoringException(const std::string & source,const std::string & message)` |
`public  MonitoringException(int code,const std::string & source,const std::string & message)` |
`public const char * what() const` |
`public  ~MonitoringException() = default` |

## Members

### `public  MonitoringException(const std::string & source,const std::string & message)`





### `public  MonitoringException(int code,const std::string & source,const std::string & message)`





### `public const char * what() const`





### `public  ~MonitoringException() = default`






# class `MonitoringFactory`






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` & operator=(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` | Disables copy constructor.
`public  MonitoringFactory(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` |

## Members

### `public `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` & operator=(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete`

Disables copy constructor.



### `public  MonitoringFactory(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete`






# class `MonitoringInternalException`

```
class MonitoringInternalException
  : public std::exception
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  MonitoringInternalException(const std::string & source,const std::string & message)` |
`public  MonitoringInternalException(int code,const std::string & source,const std::string & message)` |
`public  ~MonitoringInternalException() = default` |
`public const char * what() const` |

## Members

### `public  MonitoringInternalException(const std::string & source,const std::string & message)`





### `public  MonitoringInternalException(int code,const std::string & source,const std::string & message)`





### `public  ~MonitoringInternalException() = default`





### `public const char * what() const`






# class `MonLogger`


Simple [Monitoring](#classo2_1_1monitoring_1_1Monitoring) logging class.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public template<typename T>`  <br/>`inline `[`MonLogger`](#classo2_1_1monitoring_1_1MonLogger)` & operator<<(const T & log)` | Appends value/message to the log.

## Members

### `public template<typename T>`  <br/>`inline `[`MonLogger`](#classo2_1_1monitoring_1_1MonLogger)` & operator<<(const T & log)`

Appends value/message to the log.

#### Parameters
* `log` - message





#### Returns
- this to allow chaining


# class `ProcessDetails`






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  ProcessDetails()` |
`public unsigned int getPid() const` |
`public std::string getProcessName() const` |
`public std::string getHostname() const` |

## Members

### `public  ProcessDetails()`





### `public unsigned int getPid() const`





### `public std::string getProcessName() const`





### `public std::string getHostname() const`






# class `ProcessMonitor`


Monitors current process and/or other processes running at the same machien.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  ProcessMonitor()` | Prepares externam software commands (ps)
`public  ~ProcessMonitor() = default` | Default destructor.
`public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getPidStatus()` | Generates performance metrics (stored in mPsParams vecotr)
`public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getNetworkUsage()` | Generates metrics per network interface: bytesReceived, bytesTransmitted.

## Members

### `public  ProcessMonitor()`

Prepares externam software commands (ps)



### `public  ~ProcessMonitor() = default`

Default destructor.



### `public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getPidStatus()`

Generates performance metrics (stored in mPsParams vecotr)



### `public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getNetworkUsage()`

Generates metrics per network interface: bytesReceived, bytesTransmitted.




# class `Tag`


Represent key-value metric tag.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string name` |
`public std::string value` |

## Members

### `public std::string name`





### `public std::string value`






# class `VariantVisitorAdd`

```
class VariantVisitorAdd
  : public boost::static_visitor< boost::variant< int, std::string, double, uint64_t > >
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public inline double operator()(const std::string &,const std::string &) const` | Overloads operator() to avoid operating on strings.
`public inline int operator()(const int & a,const int & b) const` | Calculates rate only when two arguments of the same type are passed.
`public inline double operator()(const double & a,const double & b) const` |
`public inline uint64_t operator()(const uint64_t & a,const uint64_t & b) const` |
`public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const` | If arguments have different type an exception is raised.

## Members

### `public inline double operator()(const std::string &,const std::string &) const`

Overloads operator() to avoid operating on strings.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`

### `public inline int operator()(const int & a,const int & b) const`

Calculates rate only when two arguments of the same type are passed.

#### Returns
calculated rate in Hz

### `public inline double operator()(const double & a,const double & b) const`





### `public inline uint64_t operator()(const uint64_t & a,const uint64_t & b) const`





### `public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const`

If arguments have different type an exception is raised.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`


# class `VariantVisitorRate`

```
class VariantVisitorRate
  : public boost::static_visitor< boost::variant< int, std::string, double, uint64_t > >
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public inline  VariantVisitorRate(int count)` | Creates variant visitor functor.
`public inline double operator()(const std::string &,const std::string &) const` | Overloads operator() to avoid operating on strings.
`public template<typename T>`  <br/>`inline double operator()(const T & a,const T & b) const` | Calculates rate only when two arguments of the same type are passed.
`public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const` | If arguments have different type an exception is raised.

## Members

### `public inline  VariantVisitorRate(int count)`

Creates variant visitor functor.

#### Parameters
* `count` timestamp difference in milliseconds

### `public inline double operator()(const std::string &,const std::string &) const`

Overloads operator() to avoid operating on strings.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`

### `public template<typename T>`  <br/>`inline double operator()(const T & a,const T & b) const`

Calculates rate only when two arguments of the same type are passed.

#### Returns
calculated rate in Hz

### `public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const`

If arguments have different type an exception is raised.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`


# class `Version`


Information about the current [Monitoring](#classo2_1_1monitoring_1_1Monitoring) version.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

## Members


# namespace `http`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``url``](#structhttp_1_1url)    |

# struct `url` {#structhttp_1_1url}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string protocol` |
`public std::string user` |
`public std::string password` |
`public std::string host` |
`public std::string path` |
`public std::string search` |
`public int port` |

## Members

### `public std::string protocol` {#structhttp_1_1url_1a67ebf4c624c5033f37e4006b0452f875}





### `public std::string user` {#structhttp_1_1url_1aba19ced7680ec353f3486d366210ede5}





### `public std::string password` {#structhttp_1_1url_1ae27e425a13d21b5aa9a3663b746ef2fb}





### `public std::string host` {#structhttp_1_1url_1afd78fea137488e647cd8423ae29b5f1e}





### `public std::string path` {#structhttp_1_1url_1a33cefe045b9268db92b0cff6c6bb43c1}





### `public std::string search` {#structhttp_1_1url_1ace782658b03ce413699f704a1c04ea0c}





### `public int port` {#structhttp_1_1url_1a051fbe43be75dd488424b0ea46e72439}






# namespace `std`

STL namespace.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
