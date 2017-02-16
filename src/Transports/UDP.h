///
/// \file UDP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_UDP_H
#define ALICEO2_MONITORING_TRANSPORTS_UDP_H

#include "TransportInterface.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{
/// Monitoring transports
namespace Transports
{

/// Transport that sends string formatted metrics via UDP
class UDP : public TransportInterface
{
  public:
    /// Constructor
    /// \param hostname      InfluxDB instance hostname
    //  \param port          InfluxDB instance port number
    UDP(const std::string &hostname, int port);

    /// Default destructor
    ~UDP() = default;
 
    /// Sends metric via UDP
    /// \param lineMessage   r-value string formated
    void send(std::string&& message) override;   

  private:
    /// Boost Asio I/O functionality
    boost::asio::io_service mIoService;

    /// UDP socket
    boost::asio::ip::udp::socket mSocket;

    /// UDP endpoint
    boost::asio::ip::udp::endpoint mEndpoint;

};

} // namespace Transports
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_TRANSPORTS_UDP_H
