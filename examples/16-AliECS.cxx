///
/// \file 16-AliECS.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include <grpc++/grpc++.h>
#include "o2control.grpc.pb.h"
#include "helpers/HttpConnection.h"
#include <boost/program_options.hpp>
#include <thread>
#include <regex>
#include "../src/Backends/InfluxDB.h"
#include "../src/Transports/HTTP.h"
#include "../src/MonLogger.h"

using o2::monitoring::MonLogger;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using o2control::EnvironmentInfo;
using namespace o2::monitoring;

class AliEcsClient {
 public:
  AliEcsClient(std::shared_ptr<Channel> channel) : mStub(o2control::Control::NewStub(channel)) {}
  void sendRunDetails(const auto& influxBackend) {
    o2control::GetEnvironmentsRequest request;
    request.set_showall(false);
    request.set_showtaskinfos(false);
    o2control::GetEnvironmentsReply reply;
    ClientContext context;
    Status status = mStub->GetEnvironments(&context, request, &reply);
    if (status.ok()) {
      MonLogger::Get() << "Status call OK" << MonLogger::End();
      for (int i = 0; i < reply.environments_size(); i++) {
        if (reply.environments(i).currentrunnumber() > 1) {
          MonLogger::Get() << "Env ID" << reply.environments(i).id() << MonLogger::End();
          auto metric = Metric{"tasks"};
          metric.addValue(reply.environments(i).numberofactivetasks(), "active").addValue(reply.environments(i).numberofinactivetasks(), "inactive");
          influxBackend->sendWithRun(metric, reply.environments(i).id(), std::to_string(reply.environments(i).currentrunnumber()));
        }
      }
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }
 private:
  std::unique_ptr<o2control::Control::Stub> mStub;
};


int main(int argc, char* argv[]) {
  boost::program_options::options_description desc("Program options");
  desc.add_options()
    ("aliecs-host", boost::program_options::value<std::string>()->required(), "AliECS hostname")
    ("aliecs-port", boost::program_options::value<unsigned short>()->required(), "AliECS port")
    ("influxdb-url", boost::program_options::value<std::string>()->required(), "InfluxDB hostname")
    ("influxdb-token", boost::program_options::value<std::string>()->required(), "InfluxDB token")
    ("influxdb-org", boost::program_options::value<std::string>()->default_value("cern"), "InfluxDB organisation")
    ("influxdb-bucket", boost::program_options::value<std::string>()->default_value("aliecs"), "InfluxDB bucket");
  boost::program_options::variables_map vm; 
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);
  MonLogger::mLoggerSeverity = o2::monitoring::Severity::Debug;
  MonLogger::Get() << "Connected to AliECS server: " << vm["aliecs-host"].as<std::string>() << ":" << vm["aliecs-port"].as<unsigned short>() << MonLogger::End();
  grpc::ChannelArguments args;
  args.SetMaxReceiveMessageSize(20*1024*1024);
  AliEcsClient client(grpc::CreateCustomChannel(
    vm["aliecs-host"].as<std::string>() + ":" + std::to_string(vm["aliecs-port"].as<unsigned short>()),
    grpc::InsecureChannelCredentials(),
    args
  ));
  auto httpTransport = std::make_unique<transports::HTTP>(
    vm["influxdb-url"].as<std::string>() + "/api/v2/write?" +
    "org=" + vm["influxdb-org"].as<std::string>() + "&" +
    "bucket=" + vm["influxdb-bucket"].as<std::string>()
  );  
  httpTransport->addHeader("Authorization: Token " + vm["influxdb-token"].as<std::string>());
  auto influxdbBackend = std::make_unique<backends::InfluxDB>(std::move(httpTransport));
  for (;;) {
    client.sendRunDetails(influxdbBackend);
    std::this_thread::sleep_for(std::chrono::seconds(15));
  }
}
