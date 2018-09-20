///
/// \file MonitoringException.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H
#define ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H

#include <exception>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Internal monitoring exception
class MonitoringException : public std::exception
{
  public:
    MonitoringException(const std::string& source, const std::string& message);
    MonitoringException(int code, const std::string& source, const std::string& message);
    ~MonitoringException() = default;
    const char* what() const throw();
  private:
    std::string message;
};

} // namespace monitoring
} // namespace o2

#endif //ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H
