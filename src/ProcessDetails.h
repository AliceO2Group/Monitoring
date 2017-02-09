///
/// \file Tags.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_PROCESSDETAILS_H
#define ALICEO2_MONITORING_CORE_PROCESSDETAILS_H

#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
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

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_PROCESSDETAILS_H
