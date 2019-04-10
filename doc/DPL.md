# Instructions for DPL users

DPL stands for [Data Processing Layer](https://github.com/AliceO2Group/AliceO2/tree/dev/Framework/Core) and it is part of ALICE O<sup>2</sup> Framework.

### Monitoring in DPL

A URL to the monitoring backend can be passed through `monitoring-url` parameter. This will redirect all the metric to the selected backend. The list of supported backends is available in the [Monitoring README](https://github.com/AliceO2Group/Monitoring#monitoring-instance) file.

Then, from the `DataProcessingDevice` source Monitoring instance can be accessed via [Monitoring service](https://github.com/AliceO2Group/AliceO2/tree/dev/Framework/Core#monitoring-service).

### Monitoring backends at CERN

There are so-called _Community Services_ available at CERN GPN network and O<sup>2</sup> developers are welcome to use them in order to store and display metrics.

#### Time-series database
A time-series database, called [InfluxDB](https://docs.influxdata.com/influxdb/v1.7/), is used for metric storage.

Pass the following Monitoring URL via `--monitoring-url` to your workflow in order to redirect metrics to _Community Services_ instance:
```
influxdb-udp://aido2mon-gpn:8089
```

#### Dashboard

Time-series can be visualised in [Grafana](https://grafana.com) using a web browser.

1. Go to: https://alice-o2-mon.cern.ch:3000
2. Sign in with CERN SSO
3. Create a new dashboard, then and a new panel and use [Query Editor](http://docs.grafana.org/features/datasources/influxdb/#query-editor) to visualise previously stored metrics.
