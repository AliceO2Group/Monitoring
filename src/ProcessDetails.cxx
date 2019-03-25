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
/// \file ProcessDetails.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ProcessDetails.h"

#include <unistd.h>

#ifdef O2_MONITORING_OS_MAC
#include <mach-o/dyld.h>
#endif

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
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
  #ifdef O2_MONITORING_OS_LINUX
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
  if (len != -1) {
    buff[len] = '\0';
    mProcessName = std::string(buff);
  }
  #endif

  #ifdef O2_MONITORING_OS_MAC
  uint32_t size = sizeof(buff);
  if (_NSGetExecutablePath(buff, &size) == 0) {
    mProcessName = std::string(buff);
  }
  #endif

  if (mProcessName.empty()) {
    mProcessName = "!";
  } else {
    mProcessName = mProcessName.substr(mProcessName.find_last_of("/") + 1);
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

} // namespace monitoring
} // namespace o2
