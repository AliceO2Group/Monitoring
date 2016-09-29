///
/// \file MonInfoLogger.h
/// \author Barthelemy von Haller
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef MONITORING_MONINFOLOGGER_H
#define MONITORING_MONINFOLOGGER_H

#include <iostream>
#include <InfoLogger/InfoLogger.hxx>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Core features of ALICE O2 Monitoring system
namespace Core
{

/// Singleton class that is used in all Monitoring classes
///
/// The aim of this class is to avoid every class in the package to define and configure its own instance of InfoLogger.
/// Independent InfoLogger instance can still be created when and if needed.
class MonInfoLogger : public AliceO2::InfoLogger::InfoLogger
{
  public:
    static MonInfoLogger &GetInstance()
    {
      static MonInfoLogger foo;
      return foo;
    }

  private:
    MonInfoLogger()
    {
      *this << "InfoLogger instance for Monitoring initialized" << AliceO2::InfoLogger::InfoLogger::endm;
    }

    virtual ~MonInfoLogger()
    {
    }

    /// Delete copy constructors
    MonInfoLogger &operator=(const MonInfoLogger &) = delete;
    MonInfoLogger(const MonInfoLogger &) = delete;

};

} // Core
} // Monitoring
} // Alice02

#endif //MONITORING_MONINFOLOGGER_H
