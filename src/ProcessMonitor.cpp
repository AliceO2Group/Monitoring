#include <chrono> 
#include <sstream>
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include "Monitoring/ProcessMonitor.h"

namespace AliceO2 {
namespace Monitoring {
namespace Core {


ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector):
collector(collector)
{
	pids.push_back((int) ::getpid());
}

ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector, int pid):
collector(collector)
{
        pids.push_back(pid);
}

ProcessMonitor::ProcessMonitor(std::shared_ptr<Collector> collector, std::vector<int> pids) : 
collector(collector), pids(pids)
{}


std::vector<std::string> ProcessMonitor::getPIDStatus(int pid)
{
        std::stringstream ss;
        ss << "ps --no-headers --pid " << pid << " -o pid,etime,time,pcpu,pmem,rsz,vsz,comm";
	std::string output = exec(ss.str().c_str());
	std::vector<std::string> params;
        boost::split(params, output, boost::is_any_of(" "), boost::token_compress_on);
	params.erase(params.begin());
	return params;
}

std::string ProcessMonitor::exec(const char* cmd)
{
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

void ProcessMonitor::threadLoop()
{
	for (;;) 
	{
		for (auto const pid : pids)
		{
			std::vector<std::string> PIDparams = getPIDStatus(pid);
			for (std::vector<std::string>::const_iterator i = PIDparams.begin(), j = labels.begin(); i != PIDparams.end(); ++i, j++)
			{
				collector->sendDirect(*i, *j);
			}
		}	
		std::this_thread::sleep_for (std::chrono::seconds(1));
	}
}

void ProcessMonitor::startMonitor()
{
	monitorThread = std::thread(&ProcessMonitor::threadLoop, this);
}
ProcessMonitor::~ProcessMonitor()
{
	if (monitorThread.joinable())
	{
		monitorThread.join();
	}
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

