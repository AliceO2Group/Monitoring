///
/// \file TCP.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TRANSPORTS_TCP_H
#define ALICEO2_MONITORING_TRANSPORTS_TCP_H

#include "TransportInterface.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{
/// Monitoring transports
namespace transports
{

/// \brief Transport that sends string formatted metrics via TCP
class TCP : public TransportInterface
{
  public:
    /// Constructor
    /// \param hostname      InfluxDB instance hostname
    /// \param port          InfluxDB instance port number
    TCP(const std::string &hostname, int port);

    /// Default destructor
    ~TCP() = default;
 
    /// Sends metric via UDP
    /// \param message   r-value string formated
    void send(std::string&& message) override;

    /// Dummy read method - Forwards read out buffer to cout
    void read();

  private:
    /// Boost Asio I/O functionality
    boost::asio::io_service mIoService;

    /// TCP socket
    boost::asio::ip::tcp::socket mSocket;

    /// TCP endpoint interator
    boost::asio::ip::tcp::resolver::iterator mEndpoint;

};

} // namespace transports
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TRANSPORTS_TCP_H
