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


#ifdef O2_MONITORING_WITH_INFOLOGGER
#include <InfoLogger/InfoLogger.hxx>
#else
#include <chrono>
#include <iomanip>
#include <iostream>
#endif

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

#ifdef O2_MONITORING_WITH_INFOLOGGER
class MonLogger : public AliceO2::InfoLogger::InfoLogger
{

 public:
  static MonLogger& Get()
  {
    // Guaranteed to be destroyed. Instantiated on first use
    static MonLogger foo;
    return foo;
  }

  static auto End() -> decltype(AliceO2::InfoLogger::InfoLogger::endm)
  {
    return AliceO2::InfoLogger::InfoLogger::endm;
  }

 private:
  MonLogger()
  {
    *this << "MonLogger initialized" << AliceO2::InfoLogger::InfoLogger::endm;
  }

  ~MonLogger() override {}

  MonLogger& operator=(const MonLogger&) = delete;
  MonLogger(const MonLogger&) = delete;
};
#else
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
    static MonLogger &Get()
    {
      static MonLogger loggerInstance;
      loggerInstance.setDate();
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

    /// Delete copy and move constructors
    MonLogger &operator=(const MonLogger &) = delete;
    MonLogger(const MonLogger &) = delete;
    MonLogger(MonLogger &&) = delete;
    MonLogger &operator=(MonLogger&&) = delete;
};
#endif

} // namespace monitoring
} // namespace o2

#endif //MONITORING_MONINFOLOGGER_H
