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

class ProcessDetails
{
  public:
    ProcessDetails();
    unsigned int getPid() const;
    std::string getProcessName() const;
    std::string getHostname() const;

  private:
    unsigned int mPid;
    std::string mHostname;
    std::string mProcessName;


    void generatePid();
    void generateProcessName();
    void generateHostname();

};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_PROCESSDETAILS_H
