///
/// \file ProcessMonitor.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/ProcessMonitor.h"
#include "Exceptions/MonitoringInternalException.h"
#include "MonLogger.h"
#include <boost/algorithm/string/classification.hpp> 
#include <chrono>
#include <sstream>
#include <cmath>
#include <fstream>
#include <utility>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

ProcessMonitor::ProcessMonitor()
{
  mPid = static_cast<unsigned int>(::getpid());
  getrusage(RUSAGE_SELF, &mPreviousGetrUsage);
  mTimeLastRun = std::chrono::high_resolution_clock::now();
}

Metric ProcessMonitor::getMemoryUsage()
{
  std::string command = "ps --no-headers -o pmem --pid " + std::to_string(mPid);
  std::string output = exec(command.c_str());
  boost::trim(output);
  return Metric{std::stod(output), "memoryUsagePercentage"};
}

std::vector<Metric> ProcessMonitor::getCpuAndContexts() {
  std::vector<Metric> metrics;
  struct rusage currentUsage;
  getrusage(RUSAGE_SELF, &currentUsage);
  auto timeNow = std::chrono::high_resolution_clock::now();
  double timePassed = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - mTimeLastRun).count();
  if (timePassed < 950) { // do not run too often
    throw MonitoringInternalException("Process Monitor getrusage", "Do not invoke more often then 1ms");
  }
  double fractionCpuUsed = (
      currentUsage.ru_utime.tv_sec*1000000.0 + currentUsage.ru_utime.tv_usec - (mPreviousGetrUsage.ru_utime.tv_sec*1000000.0 + mPreviousGetrUsage.ru_utime.tv_usec)
    + currentUsage.ru_stime.tv_sec*1000000.0 + currentUsage.ru_stime.tv_usec - (mPreviousGetrUsage.ru_stime.tv_sec*1000000.0 + mPreviousGetrUsage.ru_stime.tv_usec)
  ) / timePassed;

  metrics.emplace_back(Metric{
    static_cast<double>(std::round(fractionCpuUsed * 100.0 * 100.0 ) / 100.0), "cpuUsedPercentage"
  });
  metrics.emplace_back(Metric{
    static_cast<uint64_t>(currentUsage.ru_nivcsw - mPreviousGetrUsage.ru_nivcsw), "involuntaryContextSwitches"
  });

  mTimeLastRun = timeNow;
  mPreviousGetrUsage = currentUsage;
  return metrics;
}

std::vector<Metric> ProcessMonitor::getNetworkUsage()
{
  std::vector<Metric> metrics;
  std::ifstream infile("/proc/" + std::to_string(mPid) + "/net/dev");
  std::string line;
  std::getline(infile, line);
  std::getline(infile, line);
  while (std::getline(infile, line)) {
    if (line.find("virbr") != std::string::npos) continue;
    if (line.find("lo") != std::string::npos) continue;
    std::istringstream iss(line);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                      std::istream_iterator<std::string>{}};
    metrics.emplace_back(Metric{
      static_cast<uint64_t>(std::stoull(tokens[1])),
      "bytesReceived"}.addTags({{"if", tokens[0]}})
    );
    metrics.emplace_back(Metric{
      static_cast<uint64_t>(std::stoull(tokens[9])),
      "bytesTransmitted"}.addTags({{"if", tokens[0]}})
    );
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

} // namespace monitoring
} // namespace o2
