// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
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

#ifdef O2_MONITORING_WITH_INFOLOGGER
#include <InfoLogger/InfoLogger.hxx>
using namespace AliceO2::InfoLogger;
#else
#include <iostream>
#include <iomanip>
#endif

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// List of possible log severities
#ifdef O2_MONITORING_WITH_INFOLOGGER
enum class Severity { Error = InfoLogger::Severity::Error,  // 69
                      Warn = InfoLogger::Severity::Warning, // 87
                      Info = InfoLogger::Severity::Info,    // 73
                      Debug = InfoLogger::Severity::Debug}; // 68

#else
enum class Severity { Error = 31,
                      Warn = 33,
                      Info = 49,
                      Debug = 50};
#endif


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
    if (!mMute) {
      mStream << log;
    }
    return *this;
  }

  /// Singleton
  /// Returns Logger instance with current date and given severity
  /// \param severity - severity level
  /// \return - logger instance
  static MonLogger& Get(Severity severity = Severity::Debug)
  {
    static MonLogger loggerInstance;
    loggerInstance.logHeader(severity);
    return loggerInstance;
  }

  /// Logger severity to be set by the user
  inline static Severity mLoggerSeverity = Severity::Warn;

  /// Terminates log line
  /// return - string with color termination and new line
#ifdef O2_MONITORING_WITH_INFOLOGGER
  static auto End() -> decltype(InfoLogger::endm) { return InfoLogger::endm; }
#else
  static auto End() -> decltype("\033[0m\n") { return "\033[0m\n"; }
#endif

 private:
  /// Makes sure Debug messages are muted
  bool mMute = false;

#ifdef O2_MONITORING_WITH_INFOLOGGER
  /// InfoLogger log output stream
  InfoLogger mStream;
  void logHeader(Severity severity)
  {
    InfoLoggerContext context;
    context.setField(InfoLoggerContext::FieldName::System, "Monitoring");
    context.setField(InfoLoggerContext::FieldName::Facility, "Library");
    mStream.setContext(context);
    static InfoLogger::AutoMuteToken wToken({InfoLogger::Severity::Warning, InfoLogger::Level::Devel, -1, nullptr, -1}, 2, 60);

    mMute = (static_cast<int>(severity) >= static_cast<int>(mLoggerSeverity) || severity == Severity::Error) ? false : true;
    if (severity == Severity::Warn) {
      mStream << &wToken;
    } else {
      mStream << InfoLogger::InfoLoggerMessageOption { static_cast<InfoLogger::Severity>(severity), InfoLogger::Level::Devel, -1, __FILE__, __LINE__ };
    }
  }
#else
  /// Ostream log output stream
  std::ostream& mStream = std::cout;
  void logHeader(Severity severity)
  {
    mMute = (static_cast<std::underlying_type<Severity>::type>(severity) > static_cast<std::underlying_type<Severity>::type>(mLoggerSeverity)) ? true : false;
    *this << "\033[0;" << static_cast<int>(severity) << "m";
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    *this << std::put_time(std::localtime(&now), "%Y-%m-%d %X") << "\t" << "Monitoring" << "\t";
  }
#endif

  /// Sets cout as a log stream
  MonLogger() = default;

  /// Delete copy and move constructors
  MonLogger& operator=(const MonLogger&) = delete;
  MonLogger(const MonLogger&) = delete;
  MonLogger(MonLogger&&) = delete;
  MonLogger& operator=(MonLogger&&) = delete;
};

} // namespace monitoring
} // namespace o2

#endif //MONITORING_MONINFOLOGGER_H
