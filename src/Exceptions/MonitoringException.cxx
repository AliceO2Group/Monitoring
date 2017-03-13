///
/// \file MonitoringException.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "MonitoringException.h"
#include "../MonInfoLogger.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

MonitoringException::MonitoringException(const std::string& source, const std::string& message) :
  MonitoringException(-1, source, message) {}

MonitoringException::MonitoringException(int code, const std::string& source, const std::string& message)
{
  std::stringstream ss; 
  ss << "MonitoringException[" << source << "] (" << code << "): " << message;
  this->message = ss.str();
  MonInfoLogger::Get() << AliceO2::InfoLogger::InfoLogger::Severity::Error << message << MonInfoLogger::End();
}

const char* MonitoringException::what() const throw()
{
  return message.c_str();
}

} // Monitroing
} // AliceO2
