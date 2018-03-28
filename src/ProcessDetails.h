///
/// \file ProcessDetails.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_PROCESSDETAILS_H
#define ALICEO2_MONITORING_PROCESSDETAILS_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Collects process and machine related details such as PID, process name, hostname
class ProcessDetails
{
  public:
    /// Generates all data
    ProcessDetails();

    /// PID getter
    unsigned int getPid() const;

    /// Process name getter
    std::string getProcessName() const;

    /// Hostname getter
    std::string getHostname() const;

  private:
    /// PID
    unsigned int mPid;

    /// Hostname
    std::string mHostname;

    /// Process name
    std::string mProcessName;

    /// Retrieves PID
    void generatePid()

    /// Retrieves process name
    void generateProcessName();

    /// Retrieves hostname
    void generateHostname();
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_PROCESSDETAILS_H
