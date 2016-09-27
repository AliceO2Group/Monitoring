///
/// \file ProcessMonitor.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ProcessMonitor.h"

#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include <chrono>
#include <sstream>
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{


ProcessMonitor::ProcessMonitor()
{
  mPids.push_back( (int) ::getpid() );
  for (std::vector<std::pair<std::string, ProcessMonitorType>>::const_iterator i = psParams.begin(); i != psParams.end(); ++i) {
    mPsCommand = mPsCommand.empty() ? i->first : mPsCommand += (',' +  i->first);
  }
  mPsCommand = "ps --no-headers -o " + mPsCommand + " --pid ";
}

std::vector<std::tuple<ProcessMonitorType, std::string, std::string>> ProcessMonitor::getPidsDetails()
{
  std::vector<std::tuple<ProcessMonitorType, std::string, std::string>> allPidsParams;
  std::lock_guard<std::mutex> lock(mVectorPidLock);
  for (auto const pid : mPids) {
    std::vector<std::string> pidParams = getPidStatus(pid);
    std::vector<std::pair<std::string, ProcessMonitorType>>::const_iterator  j = psParams.begin();
    for (std::vector<std::string>::const_iterator i = pidParams.begin(); i != pidParams.end(); ++i, ++j) {
      allPidsParams.emplace_back(std::make_tuple(j->second, *i, j->first));
    }
  }
  return allPidsParams;
}


void ProcessMonitor::addPid(int pid) {
  std::lock_guard<std::mutex> lock(mVectorPidLock);
  mPids.push_back(pid);
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
  if (!pipe) throw std::runtime_error("Issue encountered when running 'ps' (popen)");
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
    }
  return result;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
