///
/// \file MonitoringFactory.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_FACTORY_H
#define ALICEO2_MONITORING_FACTORY_H

#include "Monitoring/Collector.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

class MonitoringFactory
{
  public:
    /// Disables copy constructor
    MonitoringFactory & operator=(const MonitoringFactory&) = delete;
    MonitoringFactory(const MonitoringFactory&) = delete;

    /// Provide single instance on Monitoring Collector (singleton)
    /// \return              renerence to Collector instance
    static std::unique_ptr<Collector> Get(std::string url);

  private:
    /// Private constructor disallows to create instance of Factory
    MonitoringFactory() = default;
};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_FACTORY_H
