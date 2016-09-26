///
/// \file InfoLoggerBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfoLoggerBackend.h"

#include <iostream>
#include "MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

inline unsigned long InfoLoggerBackend::convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  return std::chrono::duration_cast <std::chrono::milliseconds>(
    timestamp.time_since_epoch()
  ).count();
}

InfoLoggerBackend::InfoLoggerBackend()
{
  MonInfoLogger::GetInstance() << "InfoLogger backend initialized" << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfoLoggerBackend::send(const int value, const std::string name, const std::string entity, 
                             const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  MonInfoLogger::GetInstance() << "Monitoring : Int( " << name << ", " << value << ", " 
                               << convertTimestamp(timestamp) << ", " << entity << ")" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfoLoggerBackend::send(const double value, const std::string name, const std::string entity, 
                             const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  MonInfoLogger::GetInstance() << "Monitoring : Double( " << name << ", " << value << ", " 
                               << convertTimestamp(timestamp) << ", " << entity << ")" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfoLoggerBackend::send(const std::string value, const std::string name, const std::string entity, 
                             const std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  MonInfoLogger::GetInstance() << "Monitoring : String( " << name << ", " << value << ", "
                               << convertTimestamp(timestamp) << ", " << entity << ")"
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

void InfoLoggerBackend::send(const uint32_t value, const std::string name, const std::string entity,
                             std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  MonInfoLogger::GetInstance() << "Monitoring : Uint32_t( " << name << ", " << value << ", " 
                               << convertTimestamp(timestamp) << ", " << entity << ")" 
                               << AliceO2::InfoLogger::InfoLogger::endm;
}

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

