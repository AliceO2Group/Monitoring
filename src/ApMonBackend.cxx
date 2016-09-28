///
/// \file ApMonBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "ApMonBackend.h"
#include <iostream>
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

ApMonBackend::ApMonBackend(const std::string configurationFile)
{
  try {
    mApMon = std::unique_ptr<ApMon>(new ApMon(const_cast<char*>(configurationFile.c_str())));
  } 
  catch (...) {
    MonInfoLogger::GetInstance() << "Could not open ApMon configuration file: " << configurationFile
                                 << AliceO2::InfoLogger::InfoLogger::endm;
    throw std::runtime_error("ApMon cannot be configured");
  }
  MonInfoLogger::GetInstance() << "ApMon backend initialized" << AliceO2::InfoLogger::InfoLogger::endm;
}

inline int ApMonBackend::convertTimestamp(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

void ApMonBackend::send(int value, const std::string& name, const std::string& entity, 
                        const std::chrono::time_point<std::chrono::system_clock>& timestamp) 
{
  mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()), 
    const_cast<char*>(name.c_str()), XDR_INT32, reinterpret_cast<char*>(&value), convertTimestamp(timestamp));
}
void ApMonBackend::send(double value, const std::string& name, const std::string& entity, 
                        const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()), 
    const_cast<char*>(name.c_str()), XDR_REAL64, reinterpret_cast<char*>(&value), convertTimestamp(timestamp));
}
void ApMonBackend::send(std::string value, const std::string& name, const std::string& entity, 
                        const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
    const_cast<char*>(name.c_str()), XDR_STRING, const_cast<char*>(value.c_str()), convertTimestamp(timestamp));
}
void ApMonBackend::send(uint32_t value, const std::string& name, const std::string& entity, 
                        const std::chrono::time_point<std::chrono::system_clock>& timestamp)
{
  mApMon->sendTimedParameter(const_cast<char*>(entity.c_str()), const_cast<char*>(entity.c_str()),
    const_cast<char*>(name.c_str()), XDR_INT32, reinterpret_cast<char*>(&value), convertTimestamp(timestamp));
}
} // namespace Core
} // namespace Monitoring
} // namespace AliceO2
