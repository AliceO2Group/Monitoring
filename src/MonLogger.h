// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file MonLogger.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef MONITORING_MONINFOLOGGER_H
#define MONITORING_MONINFOLOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <fairlogger/Logger.h>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// List of possible log severities
enum class Severity { Error = 31,
                      Warn = 33,
                      Info = 49,
                      Debug = 50};

/// Simple Monitoring logging class
class MonLogger
{
 public:
  /// Appends value/message to the log
  /// \param log - message
  /// \return - this to allow chaining
  template <typename T>
  MonLogger& operator<<(const T& log)
  {
#ifdef O2_MONITORING_WITH_FAIRLOGGER
    fair::Logger(fair::Severity::INFO, MSG_ORIGIN) << log;
#else
    if (MonLogger::internalSeverity >= severity) {
      std::cout << log;
    }
#endif
    return *this;
  }

  /// Singleton
  /// Returns Logger instance with current date and given severity
  /// \param severity - severity level
  /// \return - logger instance
  static MonLogger& Get(Severity severity = Severity::Debug)
  {
    static MonLogger loggerInstance;
    loggerInstance.instanceSeverity(severity);
    loggerInstance.setDate();
    return loggerInstance;
  }

  /// Terminates log line
  /// return - string with color termination and new line
  static auto End() -> decltype("\033[0m\n")
  {
#ifdef O2_MONITORING_WITH_FAIRLOGGER
    return {};
#else
    return "\033[0m\n";
#endif

  }

  /// Currently set severity
  const Severity internalSeverity = Severity::Info;

 private:
  /// Instance severity
  Severity severity;

  MonLogger() = default;

  /// Appends current datetime to log stream
  void setDate()
  {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    *this << std::put_time(std::localtime(&now), "%Y-%m-%d %X") << "\t" << "Monitoring" << "\t";
  }

  /// Sets log color based on severity
  /// \param severity - log severity
  void instanceSeverity(Severity desiredSeverity)
  {
    severity = desiredSeverity;
#ifndef O2_MONITORING_WITH_FAIRLOGGER
    *this << "\033[0;" << static_cast<int>(severity) << "m";
#endif
  }

  /// Delete copy and move constructors
  MonLogger& operator=(const MonLogger&) = delete;
  MonLogger(const MonLogger&) = delete;
  MonLogger(MonLogger&&) = delete;
  MonLogger& operator=(MonLogger&&) = delete;
};

} // namespace monitoring
} // namespace o2

#endif //MONITORING_MONINFOLOGGER_H
