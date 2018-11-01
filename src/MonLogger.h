///
/// \file MonLogger.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef MONITORING_MONINFOLOGGER_H
#define MONITORING_MONINFOLOGGER_H

#include <InfoLogger/InfoLogger.hxx>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{


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

} // namespace monitoring
} // namespace o2

#endif //MONITORING_MONINFOLOGGER_H
