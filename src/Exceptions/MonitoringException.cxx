///
/// \file MonitoringException.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "MonitoringException.h"
#include "../MonLogger.h"

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

MonitoringException::MonitoringException(const std::string& source, const std::string& message) :
  MonitoringException(-1, source, message) {}

MonitoringException::MonitoringException(int code, const std::string& source, const std::string& message)
: message(message)
{
  MonLogger::Get(Severity::Warn) << "MonitoringException["
                       << source << "] (" << code << "): " << message << MonLogger::End();
}

const char* MonitoringException::what() const throw()
{
  return message.c_str();
}

} // namespace monitroing
} // namespace o2
