// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file MonitoringFactory.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include "Exceptions/MonitoringException.h"
#include <functional>
#include <boost/algorithm/string.hpp>
#include "MonLogger.h"
#include "UriParser/UriParser.h"

#include "Backends/StdOut.h"
#include "Backends/Noop.h"
#include "Backends/InfluxDB.h"

#include "Transports/UDP.h"
#include "Transports/Unix.h"
#include "Transports/StdOut.h"
#include "Transports/WebSocket.h"

#ifdef O2_MONITORING_WITH_APPMON
#include "Backends/ApMonBackend.h"
#endif

#ifdef O2_MONITORING_WITH_KAFKA
#include "Transports/KafkaProducer.h"
#include "Transports/KafkaConsumer.h"
#endif

#ifdef O2_MONITORING_WITH_CURL
#include "Transports/HTTP.h"
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

/// Extracts token from header add sets it as addition HTTP header
/// http://localhost:9999/?org=YOUR_ORG&bucket=YOUR_BUCKET&token=AUTH_TOKEN
/// ->
/// http://localhost:9999/api/v2/write?org=YOUR_ORG&bucket=YOUR_BUCKET
/// --header "Authorization: Token YOURAUTHTOKEN"
std::unique_ptr<Backend> getInfluxDbv2(http::url uri)
{
#ifdef O2_MONITORING_WITH_CURL
  std::string tokenLabel = "token=";
  std::string path = "/api/v2/write";
  std::string query = uri.search;

  auto tokenStart = query.find(tokenLabel);
  auto tokenEnd = query.find('&', tokenStart);
  if (tokenEnd == std::string::npos) {
    tokenEnd = query.length();
  }
  std::string token = query.substr(tokenStart + tokenLabel.length(), tokenEnd-(tokenStart + tokenLabel.length()));
  // make sure ampersand is removed
  if (tokenEnd < query.length() && query.at(tokenEnd) == '&') tokenEnd++;
  if (tokenStart > 0 && query.at(tokenStart-1) == '&') tokenStart--;
  query.erase(tokenStart, tokenEnd - tokenStart);

  auto transport = std::make_unique<transports::HTTP>("http://" + uri.host + ':' + std::to_string(uri.port) + path + '?' + query);
  transport->addHeader("Authorization: Token " + token);
  return std::make_unique<backends::InfluxDB>(std::move(transport));
#else
  throw MonitoringException("Factory", "HTTP transport is not enabled");
#endif
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
  if (uri.protocol == "ws") {
    std::string tokenLabel = "token=";
    auto tokenSearch = uri.search.find(tokenLabel);
    uri.path.erase(std::remove(uri.path.begin(), uri.path.end(), '/'), uri.path.end());
    if (tokenSearch == std::string::npos) {
      throw MonitoringException("Factory", "Grafana token is required for WebSocket backend");
    }
    std::string token = uri.search.substr(tokenSearch + tokenLabel.length());
    auto transport = std::make_unique<transports::WebSocket>(uri.host, uri.port, token, uri.path);
    return std::make_unique<backends::InfluxDB>(std::move(transport));
  }
  if (uri.protocol == "kafka") {
#ifdef O2_MONITORING_WITH_KAFKA
    auto transport = std::make_unique<transports::KafkaProducer>(uri.host, uri.port, uri.search);
    return std::make_unique<backends::InfluxDB>(std::move(transport));
#else
    throw MonitoringException("Factory", "Kafka transport is not enabled");
#endif
  }
  throw MonitoringException("Factory", "InfluxDB transport not supported: " + uri.protocol);
}

#ifdef O2_MONITORING_WITH_APPMON
std::unique_ptr<Backend> getApMon(http::url uri)
{
  return std::make_unique<backends::ApMonBackend>(uri.path);
}
#else
std::unique_ptr<Backend> getApMon(http::url /*uri*/)
{
  throw MonitoringException("Factory", "ApMon backend is not enabled");
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
  backend->setVerbosity(found->second);
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
    {"influxdb-ws", getInfluxDb},
    {"influxdbv2", getInfluxDbv2},
    {"apmon", getApMon},
    {"no-op", getNoop}
  };

  http::url parsedUrl = http::ParseHttpUrl(url);
  if (parsedUrl.protocol.empty()) {
    throw MonitoringException("Factory", "Ill-formed URI");
  }

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator == map.end()) {
    throw MonitoringException("Factory", "Unrecognized backend " + parsedUrl.protocol);
  }
  try {
    if (parsedUrl.user == "debug") {
      MonLogger::mLoggerSeverity = Severity::Debug;
    }
    auto backend = iterator->second(parsedUrl);
    if (!parsedUrl.path.empty() && parsedUrl.path != "/") {
      SetVerbosity(parsedUrl.path.substr(parsedUrl.path.rfind("/")), backend);
    }
    return backend;
  } catch (...) {
    throw MonitoringException("Factory", "Unable to create backend " + parsedUrl.url);
  }
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
#ifdef O2_MONITORING_WITH_KAFKA
std::unique_ptr<PullClient> MonitoringFactory::GetPullClient(const std::string &url, const std::vector<std::string>& topics, const std::string &label) {
  auto client = std::make_unique<transports::KafkaConsumer>(url, topics, label);
  return client;
#else
std::unique_ptr<PullClient> MonitoringFactory::GetPullClient(const std::string&, const std::vector<std::string>&, const std::string&) {
  throw MonitoringException("Factory", "Compile library with Kafka");
#endif
}

} // namespace monitoring
} // namespace o2
