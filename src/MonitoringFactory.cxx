///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <functional>
#include <stdexcept>
#include "MonLogger.h"

#include "Backends/InfoLoggerBackend.h"
#include "Backends/Flume.h"
#include "Backends/Zabbix.h"

#ifdef _WITH_APPMON
#include "Backends/ApMonBackend.h"
#endif

#ifdef _WITH_INFLUX
#include "Backends/InfluxDB.h"
#endif

namespace AliceO2 
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

template <typename T>
void addBackend(Collector* collector, http::url uri) {
    auto const position = uri.protocol.find_last_of('-');
    if (position != std::string::npos) {
      uri.protocol = uri.protocol.substr(position + 1);
    }
   collector->addBackend<T>(uri);
}

std::unique_ptr<Collector> MonitoringFactory::Get(std::string url)
{
  auto collector = std::make_unique<Collector>();
  http::url parsedUrl = http::ParseHttpUrl(url);

  if (parsedUrl.protocol.empty()) {
    throw std::runtime_error("Ill-formed URI");
  }

  static const std::map<std::string, std::function<void(Collector* collector, const http::url&)>> map = {
      {"infologger", addBackend<Backends::InfoLoggerBackend>},
      {"influxdb-udp", addBackend<Backends::InfluxDB>},
      {"influxdb-http", addBackend<Backends::InfluxDB>},
      #ifdef _WITH_APPMON
      {"monalisa", addBackend<Backends::ApMonBackend>},
      #endif
      {"flume", addBackend<Backends::Flume>},
      {"zabbix", addBackend<Backends::Zabbix>}
  };  

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator != map.end()) {
    iterator->second(collector.get(), parsedUrl);
    return collector;
  } else {
    throw std::runtime_error("Unrecognized backend " + parsedUrl.protocol);
  }
}

} // namespace Monitoring
} // namespace AliceO
