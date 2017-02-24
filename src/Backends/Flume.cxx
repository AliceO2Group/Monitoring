///
/// \file Flume.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Flume.h"
#include <string>
#include "../Transports/HTTP.h"
#include <boost/property_tree/json_parser.hpp>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring backends
namespace Backends
{

using AliceO2::InfoLogger::InfoLogger;

Flume::Flume(const std::string &hostname, int port)
{
  mTransport = std::make_unique<Transports::HTTP>("http://" + hostname + ":" + std::to_string(port));
  mThreadRunning = true;
  mDispatchThread = std::thread(&Flume::dispatchLoop, this);
}

Flume::~Flume()
{
  mThreadRunning = false;
  if (mDispatchThread.joinable()) {
    mDispatchThread.join();
  }
}

void Flume::dispatchLoop()
{
  while (mThreadRunning) {
    std::this_thread::sleep_for (std::chrono::milliseconds(500));
    if (mQueue.empty()) continue;
    boost::property_tree::ptree root;
    root.put_child("array", boost::property_tree::ptree());
    auto& rootArray = root.get_child("array");
    {
      std::lock_guard<std::mutex> lock(mQueueMutex);
      for (auto& metric : mQueue) {
        boost::property_tree::ptree header = globalHeader;
        header.put<std::string>("timestamp", std::to_string(convertTimestamp(metric.getTimestamp())));
        header.put<std::string>("name", metric.getName());
        header.put<std::string>("value", boost::lexical_cast<std::string>(metric.getValue()));
        for (const auto& tag : metric.getTags()) {
          header.put<std::string>(tag.name, tag.value);
        }
        boost::property_tree::ptree event;
        event.push_back(std::make_pair("headers", header));
        rootArray.push_back(std::make_pair("", event));
      }
      mQueue.clear();
    }
    std::stringstream ss;
    write_json(ss, root);
    std::string boostHackJSON = ss.str();
    boostHackJSON.replace(0, 15, "");
    mTransport->send(boostHackJSON.substr(0, boostHackJSON.size()-3));
  }   
}

inline unsigned long Flume::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::nanoseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void Flume::send(const Metric& metric)
{
  std::lock_guard<std::mutex> lock(mQueueMutex);
  mQueue.push_back(metric);
}

void Flume::addGlobalTag(std::string name, std::string value)
{
  globalHeader.put<std::string>(name, value);
}

} // namespace Backends
} // namespace Monitoring
} // namespace AliceO2
