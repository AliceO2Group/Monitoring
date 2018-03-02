///
/// \file MonitoringInternalException.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "MonitoringInternalException.h"
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

MonitoringInternalException::MonitoringInternalException(const std::string& source, const std::string& message) :
  MonitoringInternalException(-1, source, message) {}

MonitoringInternalException::MonitoringInternalException(int code, const std::string& source, const std::string& message)
: message(message)
{
  MonLogger::Get(Severity::Warn) << "MonitoringInternalException["
                       << source << "] (" << code << "): " << message << MonLogger::End();
}

const char* MonitoringInternalException::what() const throw()
{
  return message.c_str();
}

} // namespace monitroing
} // namespace o2
