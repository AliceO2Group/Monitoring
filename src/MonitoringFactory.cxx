///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <functional>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "MonLogger.h"
#include "UriParser/UriParser.h"

#include "Backends/InfoLoggerBackend.h"
#include "Backends/Flume.h"

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

void addInfoLogger(Monitoring* monitoring, http::url uri) {
  monitoring->addBackend(std::make_unique<Backends::InfoLoggerBackend>());
}
void addInfluxDb(Monitoring* monitoring, http::url uri) {
  auto const position = uri.protocol.find_last_of('-');
  if (position != std::string::npos) {
    uri.protocol = uri.protocol.substr(position + 1);
  }
  if (uri.protocol == "udp") {
    monitoring->addBackend(std::make_unique<Backends::InfluxDB>(uri.host, uri.port));
  }
  if (uri.protocol == "http") {
    monitoring->addBackend(std::make_unique<Backends::InfluxDB>(uri.host, uri.port, uri.search));
  }
}
void addApMon(Monitoring* monitoring, http::url uri) {
#ifdef _WITH_APPMON
  monitoring->addBackend(std::make_unique<Backends::ApMonBackend>(uri.path));
#else
  throw std::runtime_error("ApMon backend is not enabled");
#endif
}
void addFlume(Monitoring* monitoring, http::url uri) {
  monitoring->addBackend(std::make_unique<Backends::Flume>(uri.host, uri.port));
}

std::unique_ptr<Monitoring> MonitoringFactory::Get(std::string urlsString)
{
  static const std::map<std::string, std::function<void(Monitoring* monitoring, const http::url&)>> map = {
      {"infologger", addInfoLogger},
      {"influxdb-udp", addInfluxDb},
      {"influxdb-http", addInfluxDb},
      {"apmon", addApMon},
      {"flume", addFlume},
  };  

  auto monitoring = std::make_unique<Monitoring>();

  std::vector<std::string> urls;
  boost::split(urls, urlsString, boost::is_any_of(","));

  for (auto url : urls) {
    http::url parsedUrl = http::ParseHttpUrl(url);
    if (parsedUrl.protocol.empty()) {
      throw std::runtime_error("Ill-formed URI");
    }

    auto iterator = map.find(parsedUrl.protocol);
    if (iterator != map.end()) {
      iterator->second(monitoring.get(), parsedUrl);
    } else {
      throw std::runtime_error("Unrecognized backend " + parsedUrl.protocol);
    }
  }
  return monitoring;
}

} // namespace Monitoring
} // namespace AliceO
