#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../src/Transports/KafkaConsumer.h"
#include "envs.pb.h"
#include "../MonLogger.h"

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;
using namespace std::literals::string_literals;


aliceo2::envs::ActiveRunsList gActiveEnvs;
std::mutex gEnvAccess;


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

// "Loop" forever accepting new connections.
void httpServer(tcp::acceptor& acceptor, tcp::socket& socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec) {
     auto connection = std::make_shared<httpConnection>(std::move(socket));
     connection->addCallback("SHOW+RETENTION+POLICIES",
     [](http::request<http::dynamic_body>& /*request*/, http::response<http::dynamic_body>& response) {
       response.set(http::field::content_type, "application/json");
       beast::ostream(response.body()) << "{}\n";
     });
     connection->addCallback("SHOW+TAG+VALUES+FROM+runs",
     [](http::request<http::dynamic_body>& /*request*/, http::response<http::dynamic_body>& response) {
       std::string jsonPrefix = R"({"results": [{"statement_id": 0, "series": [{"name": "env_active", "columns": ["key", "value"], "values": [)";
       std::string jsonSuffix = R"(]}]}]})";
       response.set(http::field::content_type, "application/json");
       const std::lock_guard<std::mutex> lock(gEnvAccess);
       std::string envsJson;
       for (int i = 0; i < gActiveEnvs.activeruns_size(); i++) {
         envsJson += "[\"run\", \"" + std::to_string(gActiveEnvs.activeruns(i).runnumber()) + "\"],";
       }
       if (!envsJson.empty()) {
         envsJson.pop_back();
       }
       beast::ostream(response.body()) << jsonPrefix << envsJson << jsonSuffix << '\n';
     });
     connection->addCallback("SHOW+TAG+VALUES+FROM+detectors",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
       std::string jsonPrefix = R"({"results": [{"statement_id": 0, "series": [{"name": "detectors", "columns": ["key", "value"], "values": [)";
       std::string jsonSuffix = R"(]}]}]})";
       std::string runString = std::string(request.target().substr(request.target().find("WHERE+run+%3D+") + 14));
       response.set(http::field::content_type, "application/json");
       uint32_t run;
       try {
         run  = static_cast<uint32_t>(std::stoul(runString));
       } catch(...) {
         beast::ostream(response.body()) << "{}\r\n";
         return;
       }
       const std::lock_guard<std::mutex> lock(gEnvAccess);
       std::string detectorsJson;
       for (int i = 0; i < gActiveEnvs.activeruns_size(); i++) {
         if (run != gActiveEnvs.activeruns(i).runnumber()) {
           continue;
         }
         for (int j = 0; j < gActiveEnvs.activeruns(i).detectors_size(); j++) {
           detectorsJson += "[\"detector\", \"" + boost::algorithm::to_lower_copy(gActiveEnvs.activeruns(i).detectors(j)) + "\"],";
         }
         if (!detectorsJson.empty()) {
           detectorsJson.pop_back();
         }
       }
       beast::ostream(response.body()) << jsonPrefix << detectorsJson << jsonSuffix << '\n';
     });
     connection->addCallback("active_runs+WHERE+run",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
      std::string jsonPrefix = R"({"results":[{"statement_id":0,"series":[{"name":"env","columns":["time","Env ID","Run number","Detectors","State", "Run type"],"values":[)";
      std::string jsonSuffix = R"(]}]}]})";
      response.set(http::field::content_type, "application/json");
      std::string runString = std::string(request.target().substr(request.target().find("WHERE+run+%3D+") + 14));
      uint32_t run;
      try {
        run  = static_cast<uint32_t>(std::stoul(runString));
      } catch(...) {
        beast::ostream(response.body()) << "{}\r\n";
        return;
      }
      const std::lock_guard<std::mutex> lock(gEnvAccess);
      std::string envsJson;
      for (int i = 0; i < gActiveEnvs.activeruns_size(); i++) {
        if (run != gActiveEnvs.activeruns(i).runnumber()) {
          continue;
        }
        auto detectorsProto = gActiveEnvs.activeruns(i).detectors();
        std::vector<std::string> detectors(detectorsProto.begin(), detectorsProto.end());
        envsJson += "[" + std::to_string(gActiveEnvs.activeruns(i).enterstatetimestamp()) + ", \""
                 + gActiveEnvs.activeruns(i).environmentid() + "\", "
                 + std::to_string(gActiveEnvs.activeruns(i).runnumber()) + ", \""
                 + boost::algorithm::join(detectors, " ") + "\", \""
                 + gActiveEnvs.activeruns(i).state() + "\", \""
                 + gActiveEnvs.activeruns(i).runtype() + "\"]";
      }
      beast::ostream(response.body()) << jsonPrefix << envsJson << jsonSuffix << '\n';
     });
     connection->start();
    }
    httpServer(acceptor, socket);
  });
}

// Deserialize active runs
void deserializeActiveRuns(const std::string& lastActiveRunMessage)
{
  aliceo2::envs::ActiveRunsList activeRuns;
  activeRuns.ParseFromString(lastActiveRunMessage);
  if (activeRuns.activeruns_size() == 0) {
    MonLogger::Get() << "Empty active runs" << MonLogger::End();
  } else {
    for (int i = 0; i < activeRuns.activeruns_size(); i++) {
      MonLogger::Get() << "Active run: " << activeRuns.activeruns(i).runnumber() << " ("
                       << activeRuns.activeruns(i).environmentid() << ")" << MonLogger::End();
    }
  }
  const std::lock_guard<std::mutex> lock(gEnvAccess);
  gActiveEnvs = activeRuns;
}

int main(int argc, char* argv[]) {
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("kafka-host", boost::program_options::value<std::string>()->required(), "Kafka host")
    ("kafka-topics", boost::program_options::value<std::vector<std::string>>()->multitoken()->required(), "Kafka topics")
    ("http-port", boost::program_options::value<unsigned short>()->default_value(8086), "HTTP server bind port");
  boost::program_options::variables_map vm; 
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);
  unsigned short port = vm["http-port"].as<unsigned short>();

  MonLogger::Get() << "Using Kafka instance: " << vm["kafka-host"].as<std::string>() << ":9092 and HTTP server port: " << port << MonLogger::End();
  std::thread webServerThread([&port](){
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    boost::asio::io_context ioc{1};
    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    httpServer(acceptor, socket);
    ioc.run();
  });

  auto kafkaConsumer = std::make_unique<o2::monitoring::transports::KafkaConsumer>(
    vm["kafka-host"].as<std::string>() + ":9092", std::vector<std::string>{vm["kafka-topics"].as<std::vector<std::string>>()}, "kafka-aliecs-active-envs"
  );
  for (;;) {
    auto serializedRuns = kafkaConsumer->pull();
    if (!serializedRuns.empty()) {
      deserializeActiveRuns(serializedRuns.back().second);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
