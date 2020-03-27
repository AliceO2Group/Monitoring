// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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

#include "Backends/StdOut.h"
#include "Backends/Noop.h"
#include "Backends/InfluxDB.h"

#include "Transports/UDP.h"
#include "Transports/Unix.h"
#include "Transports/StdOut.h"

#ifdef O2_MONITORING_WITH_APPMON
#include "Backends/ApMonBackend.h"
#endif

#ifdef O2_MONITORING_WITH_KAFKA
#include "Transports/Kafka.h"
#endif

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

static const std::map<std::string, Verbosity> verbosities = {
  {"/prod", Verbosity::Prod},
  {"/info", Verbosity::Info},
  {"/debug", Verbosity::Debug}};

std::unique_ptr<Backend> getStdOut(http::url uri)
{
  if (uri.search.size() > 0) {
    return std::make_unique<backends::StdOut>(uri.search);
  } else {
    return std::make_unique<backends::StdOut>();
  }
}

std::unique_ptr<Backend> getInfluxDb(http::url uri)
{
  auto const position = uri.protocol.find_last_of('-');
  if (position != std::string::npos) {
    uri.protocol = uri.protocol.substr(position + 1);
  }
  if (uri.protocol == "udp") {
    auto transport = std::make_unique<transports::UDP>(uri.host, uri.port);
    return std::make_unique<backends::InfluxDB>(std::move(transport));
  }
  if (uri.protocol == "unix") {
    std::string path = uri.path;
    auto found = std::find_if(begin(verbosities), end(verbosities),
                              [&](const auto& s) { return path.find(s.first) != std::string::npos; });
    if (found != end(verbosities)) {
      path.erase(path.rfind('/'));
    }
    auto transport = std::make_unique<transports::Unix>(path);
    return std::make_unique<backends::InfluxDB>(std::move(transport));
  }
  if (uri.protocol == "stdout") {
    auto transport = std::make_unique<transports::StdOut>();
    return std::make_unique<backends::InfluxDB>(std::move(transport));
  }
  if (uri.protocol == "kafka") {
    #ifdef O2_MONITORING_WITH_KAFKA
    auto transport = std::make_unique<transports::Kafka>(uri.host, uri.port, uri.search);
    return std::make_unique<backends::InfluxDB>(std::move(transport));
    #else
    throw std::runtime_error("Kafka transport is not enabled");
    #endif
  }
  throw std::runtime_error("InfluxDB transport not supported: " + uri.protocol);
}

#ifdef O2_MONITORING_WITH_APPMON
std::unique_ptr<Backend> getApMon(http::url uri)
{
  return std::make_unique<backends::ApMonBackend>(uri.path);
}
#else
std::unique_ptr<Backend> getApMon(http::url /*uri*/)
{
  throw std::runtime_error("ApMon backend is not enabled");
}
#endif

std::unique_ptr<Backend> getNoop(http::url /*uri*/)
{
  return std::make_unique<backends::Noop>();
}

void MonitoringFactory::SetVerbosity(std::string selected, std::unique_ptr<Backend>& backend)
{
  auto found = verbosities.find(selected);
  if (found == verbosities.end()) {
    return;
  }
  backend->setVerbosisty(found->second);
  MonLogger::Get() << "...verbosity set to "
                   << static_cast<std::underlying_type<Verbosity>::type>(found->second)
                   << MonLogger::End();
}

std::unique_ptr<Backend> MonitoringFactory::GetBackend(std::string& url)
{
  static const std::map<std::string, std::function<std::unique_ptr<Backend>(const http::url&)>> map = {
    {"infologger", getStdOut},
    {"stdout", getStdOut},
    {"influxdb-udp", getInfluxDb},
    {"influxdb-unix", getInfluxDb},
    {"influxdb-stdout", getInfluxDb},
    {"influxdb-kafka", getInfluxDb},
    {"apmon", getApMon},
    {"no-op", getNoop}
  };

  http::url parsedUrl = http::ParseHttpUrl(url);
  if (parsedUrl.protocol.empty()) {
    throw std::runtime_error("Ill-formed URI");
  }

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator == map.end()) {
    throw std::runtime_error("Unrecognized backend " + parsedUrl.protocol);
  }

  auto backend = iterator->second(parsedUrl);
  if (!parsedUrl.path.empty() && parsedUrl.path != "/") {
    SetVerbosity(parsedUrl.path.substr(parsedUrl.path.rfind("/")), backend);
  }
  return backend;
}

std::unique_ptr<Monitoring> MonitoringFactory::Get(std::string urlsString)
{
  auto monitoring = std::make_unique<Monitoring>();

  std::vector<std::string> urls;
  boost::split(urls, urlsString, boost::is_any_of(","));

  for (auto url : urls) {
    monitoring->addBackend(MonitoringFactory::GetBackend(url));
  }
  return monitoring;
}

} // namespace monitoring
} // namespace o2
