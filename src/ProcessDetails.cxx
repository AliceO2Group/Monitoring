///
/// \file Tags.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ProcessDetails.h"

#include <unistd.h>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

ProcessDetails::ProcessDetails()
{
  generatePid();
  generateProcessName();
  generateHostname();
}

inline void ProcessDetails::generatePid()
{
  mPid = ::getpid();
}

inline void ProcessDetails::generateProcessName()
{
  char buff[255];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
  if (len != -1) {
    buff[len] = '\0';
    mProcessName = std::string(buff);
    mProcessName = mProcessName.substr(mProcessName.find_last_of("/") + 1);
  }
  else {
    mProcessName = "!";
  }
}

inline void ProcessDetails::generateHostname()
{
  char hostname[255];
  gethostname(hostname, 255);
  mHostname = std::string(hostname);
}

unsigned int ProcessDetails::getPid() const
{
  return mPid;
}

std::string ProcessDetails::getProcessName() const
{
  return mProcessName;
}

std::string ProcessDetails::getHostname() const
{
  return mHostname;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

