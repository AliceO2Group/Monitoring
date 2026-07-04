#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;


/// Helper class to represent HTTP connection
class httpConnection : public std::enable_shared_from_this<httpConnection>
{
public:
  httpConnection(tcp::socket socket) : mSocket(std::move(socket)) {}

  // Initiate the asynchronous operations associated with the connection.
  void start() {
    readRequest();
    checkDeadline();
  }

  void addCallback(std::string_view path, std::function<void(http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response)> callback) {
    mCallbacks.insert({path, callback});
  }

private:
  tcp::socket mSocket;
  beast::flat_buffer mBuffer{8192};
  http::request<http::dynamic_body> mRequest;
  http::response<http::dynamic_body> mResponse;
  boost::asio::steady_timer mDeadline{mSocket.get_executor(), std::chrono::seconds(5)};
  std::map<std::string_view, std::function<void(http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response)>> mCallbacks;

  // Asynchronously receive a complete request message.
  void readRequest() {
    auto self = shared_from_this();
    http::async_read(mSocket, mBuffer, mRequest, [self](beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);
      if (!ec) self->processRequest();
    });
  }

  // Determine what needs to be done with the request message.
  void processRequest() {
     mResponse.version(mRequest.version());
     mResponse.keep_alive(false);
     mResponse.result(http::status::ok);
     createResponse();
     writeResponse();
  }

  // Construct a response message based on the program state.
  void createResponse() {
    for (const auto& [key, value] : mCallbacks) {
      if (mRequest.target().find(key) != std::string_view::npos) {
        value(mRequest, mResponse);
        return;
      }
    }
    mResponse.result(http::status::not_found);
    mResponse.set(http::field::content_type, "text/plain");
    beast::ostream(mResponse.body()) << "Not found\r\n";
  }

  // Asynchronously transmit the response message.
  void writeResponse() {
    auto self = shared_from_this();
    mResponse.content_length(mResponse.body().size());
    http::async_write(mSocket, mResponse, [self](beast::error_code ec, std::size_t) {
      self->mSocket.shutdown(tcp::socket::shutdown_send, ec);
      self->mDeadline.cancel();
    });
  }

  // Check whether we have spent enough time on this connection.
  void checkDeadline() {
    auto self = shared_from_this();
    mDeadline.async_wait(
    [self](beast::error_code ec) {
      if(!ec) {
        self->mSocket.close(ec);
      }
    });
  }
};
