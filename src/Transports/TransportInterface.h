///
/// \file TransportInterface.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
#define ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H

#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring transports
namespace Transports
{

class TransportInterface
{
  public:
    TransportInterface() = default;

    virtual ~TransportInterface() = default;

    /// Sends metric in InfluxDB Line Protocol format via UDP
    /// \param lineMessage   metrc in Influx Line Protocol format
    virtual void send(std::string&& message) = 0;
};

} // namespace Transports
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
