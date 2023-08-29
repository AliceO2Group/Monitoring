///
/// \file 15-ODC.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include <grpc++/grpc++.h>
#include "odc.grpc.pb.h"
#include "helpers/HttpConnection.h"
#include <boost/program_options.hpp>
#include <thread>
#include "../src/MonLogger.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using odc::ODC;
using odc::StatusRequest;
using odc::StatusReply;

using odc::StateRequest;
using odc::StateReply;
using odc::GeneralReply;
using o2::monitoring::MonLogger;

std::mutex gMapAccess;

struct OdcStats {
  int EpnCount;
  int FailedTasks;
  std::string State;
};

std::map<std::string, OdcStats> gStats;

void httpServer(tcp::acceptor& acceptor, tcp::socket& socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec) {
     auto connection = std::make_shared<httpConnection>(std::move(socket));
     connection->addCallback("SHOW+RETENTION+POLICIES",
     [](http::request<http::dynamic_body>& /*request*/, http::response<http::dynamic_body>& response) {
       response.set(http::field::content_type, "application/json");
       beast::ostream(response.body()) << "{}\n";
     });
     connection->addCallback("SHOW+measurements",
     [](http::request<http::dynamic_body>& /*request*/, http::response<http::dynamic_body>& response) {
       response.set(http::field::content_type, "application/json");
       beast::ostream(response.body()) << R"({"results":[{"statement_id":0,"series":[{"name":"measurements","columns":["name"],"values":[["odc"]]}]}]}\n)";
     });
     connection->addCallback("odc_status+WHERE+partitionid",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
      std::string jsonPrefix = R"({"results":[{"statement_id":0,"series":[{"name":"odc","columns":["time","State","EPN count","Failed tasks"],"values":[)";
      std::string jsonSuffix = R"(]}]}]})";
      response.set(http::field::content_type, "application/json");
      std::string id = std::string(request.target().substr(request.target().find("WHERE+partitionid+%3D+") + 22));
      std::string odcStatJson;
      const std::lock_guard<std::mutex> lock(gMapAccess);
      if (gStats.find(id) != gStats.end()) {
        odcStatJson += "[" + std::to_string(0) + ", \""
          + gStats.at(id).State + "\", \""
          + std::to_string(gStats.at(id).EpnCount) + "\", \""
          + std::to_string(gStats.at(id).FailedTasks) + "\"]";
      }
      beast::ostream(response.body()) << jsonPrefix << odcStatJson << jsonSuffix << '\n';
     });
     connection->start();
    }   
    httpServer(acceptor, socket);
  });
}

class OdcClient {
 public:
  OdcClient(std::shared_ptr<Channel> channel) : mStub(ODC::NewStub(channel)) {}
  void getStatus() {
    gStats.clear();
    StatusRequest request;
    request.set_running(true);
    StatusReply reply;
    ClientContext context;
    Status status = mStub->Status(&context, request, &reply);
    if (status.ok()) {
      MonLogger::Get() << "Status call OK" << MonLogger::End();
      for (int i = 0; i < reply.partitions_size(); i++) {
        auto partitionId = reply.partitions(i).partitionid();
        OdcStats stats;
        stats.State = reply.partitions(i).state();
        getRunState(partitionId, stats);
        const std::lock_guard<std::mutex> lock(gMapAccess);
        gStats.insert({partitionId, stats});
      }
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }
  void getRunState(const std::string& partitionId, OdcStats& stats) {
    StateRequest request;
    request.set_detailed(true);
    request.set_partitionid(partitionId);
    StateReply reply;
    ClientContext context;
    Status status = mStub->GetState(&context, request, &reply);
    if (status.ok()) {
      MonLogger::Get() << "State call for " << partitionId << " OK" << MonLogger::End();
      unsigned int failedCount = 0;
      std::unordered_set<std::string> uniqueEpns{};
      for (int i = 0; i < reply.devices_size(); i++) {
        if (reply.devices(i).state() == "ERROR") {
          failedCount++;
        }
        uniqueEpns.insert(reply.devices(i).host());
      }
      stats.EpnCount = uniqueEpns.size();
      stats.FailedTasks = failedCount;
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<ODC::Stub> mStub;
};


int main(int argc, char* argv[]) {
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("odc-host", boost::program_options::value<std::string>()->required(), "ODC hostname")
    ("odc-port", boost::program_options::value<unsigned short>()->required(), "ODC port")
    ("http-port", boost::program_options::value<unsigned short>()->default_value(8088), "HTTP server bind port");
  boost::program_options::variables_map vm; 
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);
  unsigned short port = vm["http-port"].as<unsigned short>();

  MonLogger::mLoggerSeverity = o2::monitoring::Severity::Debug;
  MonLogger::Get() << "Connected to ODC server: " << vm["odc-host"].as<std::string>() << ":" << vm["odc-port"].as<unsigned short>() << "; serving HTTP on port: " << port << MonLogger::End();
  std::thread webServerThread([&port](){
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    boost::asio::io_context ioc{1};
    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    httpServer(acceptor, socket);
    ioc.run();
  }); 
  OdcClient client(grpc::CreateChannel(vm["odc-host"].as<std::string>() + ":" + std::to_string(vm["odc-port"].as<unsigned short>()), grpc::InsecureChannelCredentials()));
  for (;;) {
    client.getStatus();
    std::this_thread::sleep_for(std::chrono::seconds(15));
  }
}
