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

#ifdef O2_MONITORING_WITH_INFOLOGGER
#include <InfoLogger/InfoLogger.hxx>
using namespace AliceO2::InfoLogger;
#endif

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
    if (MonLogger::LogSeverity >= severity) {
      mStream << log;
    }
    return *this;
  }

  /// Singleton
  /// Returns Logger instance with current date and given severity
  /// \param severity - severity level
  /// \return - logger instance
  static MonLogger& Get(Severity desiredSeverity = Severity::Debug)
  {
    static MonLogger loggerInstance;
    loggerInstance.setSeverity(desiredSeverity);
#ifndef O2_MONITORING_WITH_INFOLOGGER
    loggerInstance.internalLogHeader();
#endif
    return loggerInstance;
  }

  /// Terminates log line
  /// return - string with color termination and new line
#ifdef O2_MONITORING_WITH_INFOLOGGER
  static auto End() -> decltype(InfoLogger::endm) { return InfoLogger::endm; }
#else
  static auto End() -> decltype("\033[0m\n") { return "\033[0m\n"; }
#endif

  /// Currently set severity
  const Severity LogSeverity = Severity::Info;

 private:
  /// Instance severity
  Severity severity;

  /// Log stream
#ifdef O2_MONITORING_WITH_INFOLOGGER
  InfoLogger mStream;
#else
  std::ostream& mStream = std::cout;
#endif

  /// Sets cout as a log stream
  MonLogger() = default;

  /// Stream severity setter
  void setSeverity(Severity desiredSeverity)
  {
    severity = desiredSeverity;
  }

  /// Appends severity and current datetime to log stream
  void internalLogHeader()
  {
    *this << "\033[0;" << static_cast<int>(severity) << "m";
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    *this << std::put_time(std::localtime(&now), "%Y-%m-%d %X") << "\t" << "Monitoring" << "\t";
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
