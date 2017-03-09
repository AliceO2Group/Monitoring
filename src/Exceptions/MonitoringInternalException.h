///
/// \file MonitoringInternalException.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H
#define ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H

#include <exception>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

class MonitoringInternalException : public std::exception
{
  public:
    MonitoringInternalException(const std::string& source, const std::string& message);
    MonitoringInternalException(int code, const std::string& source, const std::string& message);
    ~MonitoringInternalException() = default;
    const char* what() const throw();
  private:
    std::string message;
};

} // Monitoring
} // AliceO2

#endif //ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H
