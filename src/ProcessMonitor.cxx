///
/// \file ProcessMonitor.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ProcessMonitor.h"

#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include <chrono>
#include "MonInfoLogger.h"

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

std::vector<Metric> ProcessMonitor::getPidsDetails()
{
  std::vector<Metric> metrics;
  std::vector<std::string> pidParams = getPidStatus(mPid);
  auto j = mPsParams.begin();
  for (auto i = pidParams.begin(); i != pidParams.end(); ++i, ++j) {
     if (j->second == MetricType::DOUBLE) {
       metrics.emplace_back(Metric{boost::lexical_cast<double>(*i), j->first});
     }
     else if (j->second == MetricType::INT) {
       metrics.emplace_back(Metric{boost::lexical_cast<int>(*i), j->first});
     }
     else {
       metrics.emplace_back(Metric{*i, j->first});
     }
  }
  return metrics;
}

std::vector<std::string> ProcessMonitor::getPidStatus(int pid)
{
  std::string command = mPsCommand + std::to_string(pid);
  std::string output = exec(command.c_str());
  std::vector<std::string> params;
  boost::trim(output);
  boost::split(params, output, boost::is_any_of("\t "), boost::token_compress_on);
  return params;
}

std::string ProcessMonitor::exec(const char* cmd)
{
  char buffer[128];
  std::string result = "";
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("Issue encountered when running 'ps' (popen)");
  }
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
  }
  return result;
}

} // namespace Monitoring
} // namespace AliceO2
