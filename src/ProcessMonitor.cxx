///
/// \file ProcessMonitor.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ProcessMonitor.h"
#include "Exceptions/MonitoringInternalException.h"
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include <chrono>
#include "MonLogger.h"
#include <sstream>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

ProcessMonitor::ProcessMonitor()
{
  mPid = static_cast<unsigned int>(::getpid());
  for (auto const param : mPsParams) {
    mPsCommand = mPsCommand.empty() ? param.first : mPsCommand += (',' +  param.first);
  }
  mPsCommand = "ps --no-headers -o " + mPsCommand + " --pid ";
}

std::vector<Metric> ProcessMonitor::getNetworkUsage()
{
  std::vector<Metric> metrics;
  std::stringstream ss;
  // get bytes received and transmitted per interface
  ss << "cat /proc/" << mPid << "/net/dev | tail -n +3 |awk ' {print $1 $2 \":\" $10}'";
  std::string output = exec(ss.str().c_str());
  // for each line (each network interfrace)
  std::istringstream iss(output);
  for (std::string line; std::getline(iss, line); ) {
    auto position = line.find(":");
    auto secondPosition = line.find(":", position + 1);
    metrics.emplace_back(Metric{
      std::stoull(line.substr(position + 1, secondPosition - position - 1)),
      "bytesReceived"}.addTags({{"if", line.substr(0, position)}})
    );
    metrics.emplace_back(Metric{
      std::stoull(line.substr(secondPosition + 1, line.size())),
      "bytesTransmitted"}.addTags({{"if", line.substr(0, position)}})
    );
  }
  return metrics;
}

std::vector<Metric> ProcessMonitor::getPidStatus()
{
  std::vector<Metric> metrics;
  std::string command = mPsCommand + std::to_string(mPid);
  std::string output = exec(command.c_str());

  // split output into std vector
  std::vector<std::string> pidParams;
  boost::trim(output);
  boost::split(pidParams, output, boost::is_any_of("\t "), boost::token_compress_on);
  
  // parse output, cast to propriate types
  auto j = mPsParams.begin();
  for (auto i = pidParams.begin(); i != pidParams.end(); ++i, ++j) {
     if (j->second == MetricType::DOUBLE) {
       metrics.emplace_back(Metric{std::stod(*i), j->first});
     }
     else if (j->second == MetricType::INT) {
       metrics.emplace_back(Metric{std::stoi(*i), j->first});
     }
     else {
       metrics.emplace_back(Metric{*i, j->first});
     }
  }

  return metrics;
}

std::string ProcessMonitor::exec(const char* cmd)
{
  char buffer[128];
  std::string result = "";
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw MonitoringInternalException("Process Monitor exec", "Issue encountered when running 'ps' (popen)");
  }
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
  }
  return result;
}

} // namespace Monitoring
} // namespace AliceO2
