#include <chrono> 
#include <sstream>
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include "Monitoring/ProcessMonitor.h"
#include "Monitoring/MonInfoLogger.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {


ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector, ConfigFile &configFile):
collector(collector)
{
  
  pids.push_back((int) ::getpid());
  preparePsOptions();
  interval = configFile.getValue<int>("ProcessMonitor.interval");
  startMonitor();
}

ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector, ConfigFile &configFile, int pid):
collector(collector)
{
  pids.push_back(pid);
  preparePsOptions();
  interval = configFile.getValue<int>("ProcessMonitor.interval");
  startMonitor();
}

ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector, ConfigFile &configFile, std::vector<int> pids) : 
collector(collector), pids(pids)
{
  preparePsOptions();
  interval = configFile.getValue<int>("ProcessMonitor.interval");
  startMonitor();
}


void ProcessMonitor::preparePsOptions()
{
  for (std::vector<std::pair<std::string, int>>::const_iterator i = params.begin(); i != params.end(); ++i) {
    options = options.empty() ? i->first : options += (',' +  i->first);
  }
}

std::vector<std::string> ProcessMonitor::getPIDStatus(int pid)
{
  std::string command = "ps --no-headers --pid " + std::to_string(pid) + " -o " + options;
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

void ProcessMonitor::threadLoop()
{
  try {
    for (;;) {
      for (auto const pid : pids) {
        std::vector<std::string> PIDparams = getPIDStatus(pid);
        std::vector<std::pair<std::string, int>>::const_iterator  j = params.begin();
        for (std::vector<std::string>::const_iterator i = PIDparams.begin(); i != PIDparams.end(); ++i, j++) {
          switch (j->second) {
            case 0 : collector->sendDirect( std::stoi(*i), j->first);
                     break;
            case 1 : collector->sendDirect( std::stod(*i), j->first);
                     break;
            case 2 : collector->sendDirect(*i, j->first);
                     break;
          }
        }
      }
      std::this_thread::sleep_for (std::chrono::seconds(interval));
    }
  } catch (runtime_error& e) {
    MonInfoLogger::GetInstance() << "Process Monitoritor failed to sent data: " << e.what() 
                                 << AliceO2::InfoLogger::InfoLogger::endm;
  }
}

void ProcessMonitor::startMonitor()
{
  monitorThread = std::thread(&ProcessMonitor::threadLoop, this);
}
ProcessMonitor::~ProcessMonitor()
{
  if (monitorThread.joinable()) {
    monitorThread.join();
  }
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

