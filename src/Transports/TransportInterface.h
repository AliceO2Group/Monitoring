///
/// \file TransportInterface.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
#define ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// Transport interface for backends
///
/// Represents transport protocol for backends that already formatted metrcis can be sent by
/// Eg. TCP, UDP or HTTP
class TransportInterface
{
  public:
    TransportInterface() = default;

    virtual ~TransportInterface() = default;

    /// Sends metric via given transport
    /// \param message   r-value to string formatted metric
    virtual void send(std::string&& message) = 0;
};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_TRANSPORTINTERFACE_H
