///
/// \file MonLogger.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef MONITORING_MONINFOLOGGER_H
#define MONITORING_MONINFOLOGGER_H

#include <iostream>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

/// Simple Monitoring logging class
class MonLogger
{
  public:
    template <typename T>
    MonLogger &operator<<(const T &log) {
        mStream << log;
        return *this;
    }

    MonLogger &operator<<(std::ostream& (*log) (std::ostream&)) {
        mStream << log;
        return *this;
    }
    static MonLogger &Get()
    {
      static MonLogger loggerInstance;
      return loggerInstance;
    }
    static auto End() -> decltype("\n")
    {
      return "\n";
    }

  private:
    std::ostream &mStream;
    MonLogger(std::ostream &oStream = std::cout): mStream(oStream) {};
    
    /// Delete copy and move constructors
    MonLogger &operator=(const MonLogger &) = delete;
    MonLogger(const MonLogger &) = delete;
    MonLogger(MonLogger &&) = delete;
    MonLogger &operator=(MonLogger&&) = delete;

};

} // Monitoring
} // Alice02

#endif //MONITORING_MONINFOLOGGER_H
