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

# class `ApMonBackend` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend}

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

### `public  ApMonBackend(const std::string & path)` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1afb3850ac45d82e127744b413df60955d}

Constructs AppMon backend.

#### Parameters
* `configurationFile` filepath to [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend) configuration file

### `public  ~ApMonBackend() = default` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1aa4fa3d5079b9fcbe69f0542cda51e7b2}

Default destructor.



### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1a0af4c4214f6487e58e897278a679aeb6}

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1a218fd93ee7cb6e2f8d4aebfd29666b94}

Sends metric via MonALISA [ApMonBackend](#classo2_1_1monitoring_1_1backends_1_1ApMonBackend)'s intances is type-aware therefore cast of metric value is needed.

#### Parameters
* `metric` reference to metric object:

### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1a109c01c6a6588cce5a042ccbf04fc175}

Sends grouped metrics under common measuremet name.

#### Parameters
* `name` measurement name 


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)` {#classo2_1_1monitoring_1_1backends_1_1ApMonBackend_1ac67fdb83968c36c6fa9860f61672eb84}

Extends entity value.

#### Parameters
* `name` tag name (unused) 


* `value` tag value that is concatenated to entity string


# class `Flume` {#classo2_1_1monitoring_1_1backends_1_1Flume}

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

### `public  Flume(const std::string & host,unsigned int port)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1ae8db937ff11bd107cace9dd942bf7174}

Constructor, uses UDP transport.

#### Parameters
* `hostname` [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) HTTP endpoint hostanme 


* `port` [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) HTTP endpoint port number

### `public  ~Flume() = default` {#classo2_1_1monitoring_1_1backends_1_1Flume_1a392d18f60fabb25e51214eca1ca0c4d7}

Default destructor.



### `public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1ab2ede4b4e371c5079adf64451059fdea}

Convert timestamp to unsigned long as required by [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume).

#### Parameters
* `chrono` time_point timestamp 





#### Returns
timestamp in nanoseconds

### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1a9a32aab2d30a849d3c66fc7f7951b0a8}

Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1a7ea1b6f72882f84d76c2e4bc37b17a39}

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1a147b5656a36c361b7e5596b75627b275}

Sends multiple metric in single packet Not supported by the backend therefore it falls back to sending metric one by one TODO: changed required in [Flume](#classo2_1_1monitoring_1_1backends_1_1Flume) Source.

#### Parameters
* `name` measurement name 


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)` {#classo2_1_1monitoring_1_1backends_1_1Flume_1a96cfbe9ce01f36d3442ef3221bce3d9e}

Adds tag.

#### Parameters
* `name` tag name 


* `value` tag value


# class `InfluxDB` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB}

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

### `public  InfluxDB(const std::string & host,unsigned int port)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1afb5c801930ff170f42948ed789d8b8a7}

Constructor for both HTTP and UDP transports.

#### Parameters
* `hostname` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint hostname 


* `port` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint port number

### `public  InfluxDB(const std::string & host,unsigned int port,const std::string & path)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1acf28c9ffd8b0bb730dc4568f7126c288}

Constructor for both HTTP and UDP transports.

#### Parameters
* `hostname` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint hostname 


* `port` [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) UDP endpoint port number 


* `path` Query path

### `public  ~InfluxDB() = default` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1a640c23ea2c605c795ceb02cd04163e1c}

Default destructor.



### `public inline unsigned long convertTimestamp(const std::chrono::time_point< std::chrono::system_clock > & timestamp)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1aec50b283b62063bf50b4e33f89ecfbd9}

Convert timestamp to unsigned long as required by [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB).

#### Parameters
* `chrono` time_point timestamp 





#### Returns
timestamp in nanoseconds

### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1a9432f9b05962004d66673e56703b9586}

Sends metric to [InfluxDB](#classo2_1_1monitoring_1_1backends_1_1InfluxDB) instance via one transport.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1ae2554d7e3467c666be33615a20ad27d4}

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1af99a7f3c8eadd32a7c8dfdd3c41b61a7}

Sends multiple values in single measurement.

#### Parameters
* `name` measurement name 


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)` {#classo2_1_1monitoring_1_1backends_1_1InfluxDB_1a91b86fd22f233def13ca3c102c5d6514}

Adds tag.

#### Parameters
* `name` tag name 


* `value` tag value


# class `InfoLoggerBackend` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend}

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

### `public  InfoLoggerBackend()` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1ae6ce75aa783952c684da7920b483cf91}

Default constructor.



### `public  ~InfoLoggerBackend() = default` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1a7fb7398658d875cd29691a75160273cf}

Default destructor.



### `public virtual void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1a2c10f2f78306934e3bef64e896e58fb9}

Sends metric to InfoLogger library.

#### Parameters
* `metric` reference to metric object

### `public virtual void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1afa4b7c80d34824bf26cdd7b02138cf9c}

Sends multiple metrics not related to each other @param metrics vector of metrics.



### `public virtual void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1a406426f4eec9d4484ad078a972b34d8c}

Sends multiple metric at once Not supported by the backend therefore it falls back to sending metric one by one.

#### Parameters
* `name` measurement name 


* `metrics` list of metrics

### `public virtual void addGlobalTag(std::string name,std::string value)` {#classo2_1_1monitoring_1_1backends_1_1InfoLoggerBackend_1a10de1d08618021501e6956d2813157b8}

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

# class `HTTP` {#classo2_1_1monitoring_1_1transports_1_1HTTP}

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

### `public  HTTP(const std::string & url)` {#classo2_1_1monitoring_1_1transports_1_1HTTP_1a3bfd3929898babadd8b90f80c2300da5}

Constructor.

#### Parameters
* `url` URL of [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) server endpoint

### `public  ~HTTP() = default` {#classo2_1_1monitoring_1_1transports_1_1HTTP_1a7a5da26d9fa25ec690c753d4a8d30df6}

Default destructor.



### `public virtual void send(std::string && post)` {#classo2_1_1monitoring_1_1transports_1_1HTTP_1a69caf6b35e7a8f27bef92b513674fc40}

Sends metric via [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP) POST.

#### Parameters
* `metric` r-value reference string formatted metric


# class `TCP` {#classo2_1_1monitoring_1_1transports_1_1TCP}

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

### `public  TCP(const std::string & hostname,int port)` {#classo2_1_1monitoring_1_1transports_1_1TCP_1a9b84231848d8e53a144b4441f0ceb51b}

Constructor.

#### Parameters
* `hostname` InfluxDB instance hostname

### `public  ~TCP() = default` {#classo2_1_1monitoring_1_1transports_1_1TCP_1a07f8d869dcf81b9db7f14c8354f50efd}

Default destructor.



### `public virtual void send(std::string && message)` {#classo2_1_1monitoring_1_1transports_1_1TCP_1a1ebb22e3478af54d51a285d4b171b62b}

Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

#### Parameters
* `lineMessage` r-value string formated

### `public void read()` {#classo2_1_1monitoring_1_1transports_1_1TCP_1aaca95df6aa4325dacfd82bbc08323e49}

Dummy read method - Forwards read out buffer to cout.




# class `TransportInterface` {#classo2_1_1monitoring_1_1transports_1_1TransportInterface}


Transport interface for backends.

Represents transport protocol for backends that already formatted metrcis can be sent by Eg. [TCP](#classo2_1_1monitoring_1_1transports_1_1TCP), [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP) or [HTTP](#classo2_1_1monitoring_1_1transports_1_1HTTP)

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  TransportInterface() = default` | 
`public virtual  ~TransportInterface() = default` | 
`public void send(std::string && message)` | Sends metric via given transport.

## Members

### `public  TransportInterface() = default` {#classo2_1_1monitoring_1_1transports_1_1TransportInterface_1abf34073739b94c24d753ec48bfaddbf0}





### `public virtual  ~TransportInterface() = default` {#classo2_1_1monitoring_1_1transports_1_1TransportInterface_1aef741cd7b381ac1293885acaefc392b8}





### `public void send(std::string && message)` {#classo2_1_1monitoring_1_1transports_1_1TransportInterface_1a157d291d9b965e4c224a0232228ecb27}

Sends metric via given transport.

#### Parameters
* `message` r-value to string formatted metric


# class `UDP` {#classo2_1_1monitoring_1_1transports_1_1UDP}

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

### `public  UDP(const std::string & hostname,int port)` {#classo2_1_1monitoring_1_1transports_1_1UDP_1a8c2d0c282e4b743d79a11b8adc896cfe}

Constructor.

#### Parameters
* `hostname` InfluxDB instance hostname

### `public  ~UDP() = default` {#classo2_1_1monitoring_1_1transports_1_1UDP_1a914b7b729746f1a1746531aea237c99d}

Default destructor.



### `public virtual void send(std::string && message)` {#classo2_1_1monitoring_1_1transports_1_1UDP_1a98abbd5b00501dd472cfeef9d0b91eaa}

Sends metric via [UDP](#classo2_1_1monitoring_1_1transports_1_1UDP).

#### Parameters
* `lineMessage` r-value string formated


# class `Backend` {#classo2_1_1monitoring_1_1Backend}


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

### `public  Backend() = default` {#classo2_1_1monitoring_1_1Backend_1a580b758b89fb3ba1c6502d8949edaa40}

Default constructor.



### `public virtual  ~Backend() = default` {#classo2_1_1monitoring_1_1Backend_1a3f48dbee9ed23635e05ca6912db289df}

Default destructor.



### `public void send(const `[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1Backend_1ad84d28199d142ff3030f6b2c7e785485}

Sends metric via backend.



### `public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1Backend_1adfbe8f3a11dcdc9feabb03ad95f26c57}

Sends multiple metrics not related to each other.



### `public void sendMultiple(std::string measurement,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1Backend_1a06a6670e66e4873a66f0f4e65a6d93f4}

Sends multiple related to each other metrics under a common name If not supported by backend, falls back into sending single metrics.



### `public void addGlobalTag(std::string name,std::string value)` {#classo2_1_1monitoring_1_1Backend_1a3d86ef12edd50153f877d2101548acf9}

Sets a tag.




# class `DerivedMetrics` {#classo2_1_1monitoring_1_1DerivedMetrics}


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

### `public std::unordered_map< std::string, `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > mStorage` {#classo2_1_1monitoring_1_1DerivedMetrics_1ab45ed6358a28e8455824704e1919ef48}

Cache of registered metrics (metric name / vector of metric pointers).



### `public  DerivedMetrics() = default` {#classo2_1_1monitoring_1_1DerivedMetrics_1a01e94a78a60246b8dad7628155bbdb83}

Default constructor.



### `public  ~DerivedMetrics() = default` {#classo2_1_1monitoring_1_1DerivedMetrics_1acb0059d3cc0c570fc042663596b0f6ae}

Default destructor.



### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` rate(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1DerivedMetrics_1a4120050c16c83e12164ab4eeb1074bcf}

Calculates rate value based on metrics stored in mCache map.

#### Parameters
* `name` metric name 





#### Returns
metric with calculated rate value

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` increment(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` & metric)` {#classo2_1_1monitoring_1_1DerivedMetrics_1aba377d880146743acf53b90d8881d25c}






# class `Metric` {#classo2_1_1monitoring_1_1Metric}


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

### `public  Metric(int value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` {#classo2_1_1monitoring_1_1Metric_1a97615d8c9a93130ca2411e29cd5c6253}

Initialize class variables.

#### Parameters
* `value` metric value (int) 


* `name` metric name 


* `timestamp` metric timestamp in milliseconds

### `public  Metric(std::string value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` {#classo2_1_1monitoring_1_1Metric_1a2acf647b8c85eef26fbe66d14e180016}

Initialize class variables.

#### Parameters
* `value` metric value (string) 


* `name` the metric name 


* `timestamp` metric timestamp in milliseconds

### `public  Metric(double value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` {#classo2_1_1monitoring_1_1Metric_1aaef8dfa0fdd28a8125796bd1358ef834}

Initialize class variables.

#### Parameters
* `value` metric value (double) 


* `name` metric name 


* `timestamp` metric timestamp in milliseconds

### `public  Metric(uint64_t value,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` {#classo2_1_1monitoring_1_1Metric_1a6f754423dbfff881a7709375de9040e4}

Initialize class variables.

#### Parameters
* `value` metric value (uint64_t) 


* `name` metric name 


* `timestamp` metric timestamp in milliseconds

### `public  Metric(boost::variant< int, std::string, double, uint64_t >,const std::string & name,std::chrono::time_point< std::chrono::system_clock > timestamp)` {#classo2_1_1monitoring_1_1Metric_1ab35d7c92d451974e63e01d9830ca74f2}

Initialize class variables, required by derived metrics logic.

#### Parameters
* `value` metric value (boost variant) 


* `name` metric name 


* `timestamp` metric timestamp in milliseconds

### `public  ~Metric() = default` {#classo2_1_1monitoring_1_1Metric_1afc9ab7a124026bad375783c53dc87f4c}

Default destructor.



### `public std::string getName() const` {#classo2_1_1monitoring_1_1Metric_1a16c2be84f197599feb972318c5c68a81}

Name getter.

#### Returns
metric name

### `public void setName(std::string name)` {#classo2_1_1monitoring_1_1Metric_1af903783a5276b948674f706108fd799b}

Name setter.

#### Parameters
* `new` name of the metric

### `public std::chrono::time_point< std::chrono::system_clock > getTimestamp() const` {#classo2_1_1monitoring_1_1Metric_1a24116571ab9a85a8ef710046de8e166c}

Timestamp getter.

#### Returns
metric timestamp

### `public boost::variant< int, std::string, double, uint64_t > getValue() const` {#classo2_1_1monitoring_1_1Metric_1a9e7d054d95e99b16ce2f4ebb29bfe5bd}

Value getter.

#### Returns
metric value

### `public int getType() const` {#classo2_1_1monitoring_1_1Metric_1a733fd9d1d115b4dd3cbd1eb39a35a075}

Value type getter.

#### Returns
type of value stores inside metric : 0 - int, 1 - std::string, 2 - double

### `public std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > getTags() const` {#classo2_1_1monitoring_1_1Metric_1aeccb3ba88c536aa65fb34b70990e095b}

[Tag](#classo2_1_1monitoring_1_1Tag) list getter.

#### Returns
vector of tags

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && setTimestamp(std::chrono::time_point< std::chrono::system_clock > & timestamp)` {#classo2_1_1monitoring_1_1Metric_1a52acc2fca79cc5737e3926087aec0a89}

Sets user defined timestamp.

#### Parameters
* `timestamp` 





#### Returns
r-value to "this" - to be able to chain methods

### `public `[`Metric`](#classo2_1_1monitoring_1_1Metric)` && addTags(std::vector< `[`Tag`](#classo2_1_1monitoring_1_1Tag)` > && tags)` {#classo2_1_1monitoring_1_1Metric_1a50734a55862b79f2c7827e54a4298477}

Add user defined tags.

#### Parameters
* `r-value` to vector of tags 





#### Returns
r-value to "this" - to be able to chain methods


# class `Monitoring` {#classo2_1_1monitoring_1_1Monitoring}


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

### `public `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` & operator=(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete` {#classo2_1_1monitoring_1_1Monitoring_1ad2c62aa9bca85c77fa19a5b933db2f5c}

Disables copy constructor.



### `public  Monitoring(const `[`Monitoring`](#classo2_1_1monitoring_1_1Monitoring)` &) = delete` {#classo2_1_1monitoring_1_1Monitoring_1a1d03167696a62ea65760f05719528ba8}





### `public  Monitoring()` {#classo2_1_1monitoring_1_1Monitoring_1af60d643d981a73feac969dcc394ac6ef}

Initializes backends based on passed configuration.

Instantiates derived metrics processor (see [DerivedMetrics](#classo2_1_1monitoring_1_1DerivedMetrics) class) and process monitor (see [ProcessMonitor](#classo2_1_1monitoring_1_1ProcessMonitor)).

### `public void addBackend(std::unique_ptr< `[`Backend`](#classo2_1_1monitoring_1_1Backend)` > backend)` {#classo2_1_1monitoring_1_1Monitoring_1a548350ec288e69ad3777cb3057bf9743}

Creates backend and appends it to backend list.



### `public  ~Monitoring()` {#classo2_1_1monitoring_1_1Monitoring_1a2c674631acd35e6c23d14ed3233ec83c}

Joins process monitor thread if possible.



### `public void send(`[`Metric`](#classo2_1_1monitoring_1_1Metric)` && metric,`[`DerivedMetricMode`](#namespaceo2_1_1monitoring_1afdc1126ed1982b9a501fb36c63110ab2)` mode)` {#classo2_1_1monitoring_1_1Monitoring_1a5419ff3a2a8a28a319b7152b6790c202}

Sends a metric to all avaliabes backends If metric has been added to DerivedMetric the derived metric is calculated (see addDerivedMetric method)

#### Parameters
* `metric` r-value to metric object

### `public void send(std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1Monitoring_1a45c29b382c7da6d0765b0f1534b12661}

Sends multiple not related to each other metrics.

#### Parameters
* `metrics` vector of metrics

### `public void sendGrouped(std::string name,std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > && metrics)` {#classo2_1_1monitoring_1_1Monitoring_1a7525bb402a486e2be32411f4aeb50ade}

Sends multiple realated to each other metrics under a common measurement name You can imagine it as a data point with multiple values If it's not supported by a backend it fallbacks into sending one by one.

#### Parameters
* `name` measurement name 


* `metrics` list of metrics

### `public void enableProcessMonitoring(const unsigned int interval)` {#classo2_1_1monitoring_1_1Monitoring_1aaf4b013523f466f6f2e15bcc321fda0e}

Enables process monitoring.

#### Parameters
* `interval` refresh interval

### `public void startTimer(std::string name)` {#classo2_1_1monitoring_1_1Monitoring_1a94e48ebb955e8715a42da4eadb5ea5f7}

Starts timing Sets a start timestamp and timeout.

#### Parameters
* `name` metric name 


* `timeout` timeout

### `public void stopAndSendTimer(std::string name)` {#classo2_1_1monitoring_1_1Monitoring_1ae05c0921322be6f254fac172506d3cc6}

Stops timing Sets stop timestamp, calculates delta and sends value.

#### Parameters
* `name` metric name

### `public void flushBuffer()` {#classo2_1_1monitoring_1_1Monitoring_1a232628a04646d1188e812ed9c4338788}

Flushes metric buffer (this can also happen when buffer is full)



### `public void enableBuffering(const unsigned int size)` {#classo2_1_1monitoring_1_1Monitoring_1a4f779ef4a0be04f177a1fb5381d1c63a}

Enables metric buffering.

#### Parameters
* `size` buffer size


# class `MonitoringException` {#classo2_1_1monitoring_1_1MonitoringException}

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

### `public  MonitoringException(const std::string & source,const std::string & message)` {#classo2_1_1monitoring_1_1MonitoringException_1a9b00c5d4502958fa005ec5d539d66f5a}





### `public  MonitoringException(int code,const std::string & source,const std::string & message)` {#classo2_1_1monitoring_1_1MonitoringException_1adc38563f0cc1cc0064560a39f3938ea0}





### `public const char * what() const` {#classo2_1_1monitoring_1_1MonitoringException_1a3ec9f57d9cb930036b1f4c6b49bc9d9f}





### `public  ~MonitoringException() = default` {#classo2_1_1monitoring_1_1MonitoringException_1a10550a9c8206463564afcd9110d93f46}






# class `MonitoringFactory` {#classo2_1_1monitoring_1_1MonitoringFactory}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` & operator=(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` | Disables copy constructor.
`public  MonitoringFactory(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` | 

## Members

### `public `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` & operator=(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` {#classo2_1_1monitoring_1_1MonitoringFactory_1aacadbff97348138dd042eec39b058a9f}

Disables copy constructor.



### `public  MonitoringFactory(const `[`MonitoringFactory`](#classo2_1_1monitoring_1_1MonitoringFactory)` &) = delete` {#classo2_1_1monitoring_1_1MonitoringFactory_1a2b1165831f32ea7119b8b22dfa14ba5e}






# class `MonitoringInternalException` {#classo2_1_1monitoring_1_1MonitoringInternalException}

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

### `public  MonitoringInternalException(const std::string & source,const std::string & message)` {#classo2_1_1monitoring_1_1MonitoringInternalException_1a32a0ce1844981a5a4c293c06c8cfef0e}





### `public  MonitoringInternalException(int code,const std::string & source,const std::string & message)` {#classo2_1_1monitoring_1_1MonitoringInternalException_1a9a2bb011cefa9b08a6810c83d794d880}





### `public  ~MonitoringInternalException() = default` {#classo2_1_1monitoring_1_1MonitoringInternalException_1a8f1d56817966c3249c99ba80b44be6df}





### `public const char * what() const` {#classo2_1_1monitoring_1_1MonitoringInternalException_1aebae03ce86f462652bbf4f8dae02ea73}






# class `MonLogger` {#classo2_1_1monitoring_1_1MonLogger}


Simple [Monitoring](#classo2_1_1monitoring_1_1Monitoring) logging class.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public template<typename T>`  <br/>`inline `[`MonLogger`](#classo2_1_1monitoring_1_1MonLogger)` & operator<<(const T & log)` | Appends value/message to the log.

## Members

### `public template<typename T>`  <br/>`inline `[`MonLogger`](#classo2_1_1monitoring_1_1MonLogger)` & operator<<(const T & log)` {#classo2_1_1monitoring_1_1MonLogger_1a248af7f99d2d663ed38cff9e6900f553}

Appends value/message to the log.

#### Parameters
* `log` - message 





#### Returns
- this to allow chaining


# class `ProcessDetails` {#classo2_1_1monitoring_1_1ProcessDetails}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  ProcessDetails()` | 
`public unsigned int getPid() const` | 
`public std::string getProcessName() const` | 
`public std::string getHostname() const` | 

## Members

### `public  ProcessDetails()` {#classo2_1_1monitoring_1_1ProcessDetails_1a49de7dfd0da3566fcc4555e328bb3e78}





### `public unsigned int getPid() const` {#classo2_1_1monitoring_1_1ProcessDetails_1a1fa651582553014fbeb6ace2fca6353e}





### `public std::string getProcessName() const` {#classo2_1_1monitoring_1_1ProcessDetails_1a84d58909953758ca51cc87d8b8bc2437}





### `public std::string getHostname() const` {#classo2_1_1monitoring_1_1ProcessDetails_1a6dafdd3f7b3f1eed2d781b7b01decd81}






# class `ProcessMonitor` {#classo2_1_1monitoring_1_1ProcessMonitor}


Monitors current process and/or other processes running at the same machien.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  ProcessMonitor()` | Prepares externam software commands (ps)
`public  ~ProcessMonitor() = default` | Default destructor.
`public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getPidStatus()` | Generates performance metrics (stored in mPsParams vecotr)
`public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getNetworkUsage()` | Generates metrics per network interface: bytesReceived, bytesTransmitted.

## Members

### `public  ProcessMonitor()` {#classo2_1_1monitoring_1_1ProcessMonitor_1ad499bd2d59f2c86fc53f927f08122460}

Prepares externam software commands (ps)



### `public  ~ProcessMonitor() = default` {#classo2_1_1monitoring_1_1ProcessMonitor_1aac2fbc7b1a2d534970acbf33f0cd0de8}

Default destructor.



### `public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getPidStatus()` {#classo2_1_1monitoring_1_1ProcessMonitor_1a6b1a06f0ad89841c78b02bec71be98cd}

Generates performance metrics (stored in mPsParams vecotr)



### `public std::vector< `[`Metric`](#classo2_1_1monitoring_1_1Metric)` > getNetworkUsage()` {#classo2_1_1monitoring_1_1ProcessMonitor_1a324620db7c60212b570851127be23fba}

Generates metrics per network interface: bytesReceived, bytesTransmitted.




# class `Tag` {#classo2_1_1monitoring_1_1Tag}


Represent key-value metric tag.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string name` | 
`public std::string value` | 

## Members

### `public std::string name` {#classo2_1_1monitoring_1_1Tag_1ac8a992685a9545cbcb794127c19617f1}





### `public std::string value` {#classo2_1_1monitoring_1_1Tag_1a4d7ba5e0528755167509fc5fd5d96e7e}






# class `VariantVisitorAdd` {#classo2_1_1monitoring_1_1VariantVisitorAdd}

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

### `public inline double operator()(const std::string &,const std::string &) const` {#classo2_1_1monitoring_1_1VariantVisitorAdd_1a30a95d7eb2c899178cf033dbec57779f}

Overloads operator() to avoid operating on strings.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`

### `public inline int operator()(const int & a,const int & b) const` {#classo2_1_1monitoring_1_1VariantVisitorAdd_1a24e94ff98fe0e51aab9edfc6a3d1f091}

Calculates rate only when two arguments of the same type are passed.

#### Returns
calculated rate in Hz

### `public inline double operator()(const double & a,const double & b) const` {#classo2_1_1monitoring_1_1VariantVisitorAdd_1aa412189abcb59f8f22deaa30b490e631}





### `public inline uint64_t operator()(const uint64_t & a,const uint64_t & b) const` {#classo2_1_1monitoring_1_1VariantVisitorAdd_1ae1c4c4371154b1f890edba94b5676aab}





### `public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const` {#classo2_1_1monitoring_1_1VariantVisitorAdd_1a76b18fd12010a596f635ca072e817206}

If arguments have different type an exception is raised.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`


# class `VariantVisitorRate` {#classo2_1_1monitoring_1_1VariantVisitorRate}

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

### `public inline  VariantVisitorRate(int count)` {#classo2_1_1monitoring_1_1VariantVisitorRate_1aa336448595cb9579f7c17ea40a472002}

Creates variant visitor functor.

#### Parameters
* `count` timestamp difference in milliseconds

### `public inline double operator()(const std::string &,const std::string &) const` {#classo2_1_1monitoring_1_1VariantVisitorRate_1a541e6ae88a01d1f882510bf3940ca7f7}

Overloads operator() to avoid operating on strings.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`

### `public template<typename T>`  <br/>`inline double operator()(const T & a,const T & b) const` {#classo2_1_1monitoring_1_1VariantVisitorRate_1a90fd91e1a3c80f62952b3e13da9fee1b}

Calculates rate only when two arguments of the same type are passed.

#### Returns
calculated rate in Hz

### `public template<typename T,typename U>`  <br/>`inline double operator()(const T &,const U &) const` {#classo2_1_1monitoring_1_1VariantVisitorRate_1a3864a3f723488d8fae70b2921ee9e0f6}

If arguments have different type an exception is raised.

#### Parameters
* `[MonitoringInternalException](#classo2_1_1monitoring_1_1MonitoringInternalException)`


# class `Version` {#classo2_1_1monitoring_1_1Version}


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

