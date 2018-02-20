///
/// \file MonLogger.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef MONITORING_MONINFOLOGGER_H
#define MONITORING_MONINFOLOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// List of possible log severities
enum class Severity { Error = 31, Warn = 33, Info = 49 };

/// Simple Monitoring logging class
class MonLogger
{
  public:
    /// Appends value/message to the log
    /// \param log - message
    /// \return - this to allow chaining
    template <typename T>
    MonLogger &operator<<(const T &log) {
      mStream << log;
      return *this;
    }

    /// Singleton
    /// Returns Logger instance with current date and given severity
    /// \param severity - severity level
    /// \return - logger instance
    static MonLogger &Get(Severity severity = Severity::Info)
    {
      static MonLogger loggerInstance;
      loggerInstance.setDate();
      loggerInstance.setSeverity(severity);
      return loggerInstance;
    }

    /// Terminates log line
    /// return - string with color termination and new line
    static auto End() -> decltype("\033[0m\n")
    {
      return "\033[0m\n";
    }

  private:
    /// Log stream
    std::ostream &mStream;

    /// Sets cout as a log stream
    MonLogger(std::ostream &oStream = std::cout) : mStream(oStream)
    {}

    /// Appends current datetime to log stream
    void setDate() {
       auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
       mStream << std::put_time(std::localtime(&now), "%Y-%m-%d %X") << "\t";
    }
    
    /// Sets log color based on severity
    /// \param severity - log severity
    void setSeverity(Severity severity) {
      mStream << "\033[1;" << static_cast<int>(severity) << "m";
    }
 
    /// Delete copy and move constructors
    MonLogger &operator=(const MonLogger &) = delete;
    MonLogger(const MonLogger &) = delete;
    MonLogger(MonLogger &&) = delete;
    MonLogger &operator=(MonLogger&&) = delete;
};

} // namespace monitoring
} // namespace o2

#endif //MONITORING_MONINFOLOGGER_H
