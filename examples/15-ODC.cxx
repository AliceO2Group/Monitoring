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
#include <regex>
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
  unsigned int RecoTasks;
  unsigned int CalibTasks;
  std::unordered_map<std::string, unsigned int> TasksPerCalib;
  std::unordered_map<std::string, unsigned int> FailedTasksPerCalib;
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
     connection->addCallback("SHOW+TAG+VALUES+FROM+calibs+WHERE+partitionid",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
       std::string jsonPrefix = R"({"results": [{"statement_id": 0, "series": [{"name": "odc_calibs", "columns": ["key", "value"], "values": [)";
       std::string jsonSuffix = R"(]}]}]})";
       response.set(http::field::content_type, "application/json");
       std::string calibJson;
       std::string id = std::string(request.target().substr(request.target().find("WHERE+partitionid+%3D+") + 22));
       const std::lock_guard<std::mutex> lock(gMapAccess);
       if (gStats.find(id) != gStats.end()) {
         for (auto const& calib : gStats.at(id).TasksPerCalib) {
           calibJson += "[\"calib\", \"" + calib.first + "\"],";
         }
       }
       if (!calibJson.empty()) {
         calibJson.pop_back();
       }
       beast::ostream(response.body()) << jsonPrefix << calibJson << jsonSuffix << '\n';
     });
     connection->addCallback("odc_status+WHERE+partitionid",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
      std::string jsonPrefix = R"({"results":[{"statement_id":0,"series":[{"name":"odc","columns":["time","State","EPN count","Failed tasks", "Calib tasks", "Reco tasks"],"values":[)";
      std::string jsonSuffix = R"(]}]}]})";
      response.set(http::field::content_type, "application/json");
      std::string id = std::string(request.target().substr(request.target().find("WHERE+partitionid+%3D+") + 22));
      std::string odcStatJson;
      const std::lock_guard<std::mutex> lock(gMapAccess);
      if (gStats.find(id) != gStats.end()) {
        odcStatJson += "[" + std::to_string(0) + ", \""
          + gStats.at(id).State + "\", \""
          + std::to_string(gStats.at(id).EpnCount) + "\", \""
          + std::to_string(gStats.at(id).FailedTasks) + "\", \""
          + std::to_string(gStats.at(id).CalibTasks) + "\", \""
          + std::to_string(gStats.at(id).RecoTasks) + "\"]";
      }
      beast::ostream(response.body()) << jsonPrefix << odcStatJson << jsonSuffix << '\n';
     });
     connection->addCallback("calib_tasks+WHERE+calib",
     [](http::request<http::dynamic_body>& request, http::response<http::dynamic_body>& response) {
      std::string jsonPrefix = R"({"results":[{"statement_id":0,"series":[{"name":"calib_tasks","columns":["time","Total","Failed"],"values":[)";
      std::string jsonSuffix = R"(]}]}]})";
      response.set(http::field::content_type, "application/json");
      std::string calib = std::string(request.target().substr(request.target().find("WHERE+calib+%3D+") + 16));
      std::string calibTasksJson;
      const std::lock_guard<std::mutex> lock(gMapAccess);
      calibTasksJson += "[" + std::to_string(0);
      for (const auto& run : gStats) {
        if (run.second.TasksPerCalib.find(calib) != run.second.TasksPerCalib.end()) {
          calibTasksJson += ", ";
          calibTasksJson += std::to_string(run.second.TasksPerCalib.at(calib));
          std::cout << run.second.TasksPerCalib.at(calib) << std::endl;
          if (run.second.FailedTasksPerCalib.find(calib) != run.second.FailedTasksPerCalib.end()) {
            calibTasksJson += "," + std::to_string(run.second.FailedTasksPerCalib.at(calib));
            std::cout << run.second.FailedTasksPerCalib.at(calib) << std::endl;
          } else {
            calibTasksJson +=  ",0";
          }
        }
      }
      calibTasksJson +=  "]";
      beast::ostream(response.body()) << jsonPrefix << calibTasksJson << jsonSuffix << '\n';
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
      std::unordered_set<std::string> calibCollections{};
      unsigned int recoTasks = 0;
      unsigned int calibTasks = 0;
      std::regex rReco("_reco[0-9]+_");
      std::regex rCalib("_calib[0-9]+_");
      for (int i = 0; i < reply.devices_size(); i++) {
        if (reply.devices(i).state() == "ERROR") {
          failedCount++;
        }
        uniqueEpns.insert(reply.devices(i).host());
        if (std::regex_search(reply.devices(i).path(), rReco)) {
          recoTasks++;
        }
        if (std::regex_search(reply.devices(i).path(), rCalib)) {
          calibTasks++;
          auto calibIdx = reply.devices(i).path().find("_calib");
          auto calib = reply.devices(i).path().substr(calibIdx + 1, reply.devices(i).path().size()-calibIdx-3);
          auto it = stats.TasksPerCalib.find(calib);
          if (it != stats.TasksPerCalib.end()) {
            it->second++;
          }
          else {
            stats.TasksPerCalib.insert({calib, 1});
          }
          if (reply.devices(i).state() == "ERROR") {
            auto it = stats.FailedTasksPerCalib.find(calib);
            if (it != stats.FailedTasksPerCalib.end()) {
              it->second++;
            }
            else {
              stats.FailedTasksPerCalib.insert({calib, 1});
            }
          }
        }
      }
      const std::lock_guard<std::mutex> lock(gMapAccess);
      stats.RecoTasks = recoTasks;
      stats.CalibTasks = calibTasks;
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
