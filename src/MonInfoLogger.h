///
/// \file MonInfoLogger.h
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

using namespace std;

/// \brief  Singleton class that any class in the QC can use to log.
///
/// The aim of this class is to avoid every class in the package to define and configure its own instance of InfoLogger.
/// Independent InfoLogger instance can still be created when and if needed.
///
/// \author Barthelemy von Haller
/// \author modified by Adam Wegrzynek <adam.wegrzynek@cern.ch>
class MonInfoLogger : public AliceO2::InfoLogger::InfoLogger
{

  public:

    static MonInfoLogger &GetInstance()
    {
      // Guaranteed to be destroyed. Instantiated on first use
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
