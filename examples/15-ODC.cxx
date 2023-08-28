///
/// \file 15-ODC.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include <grpc++/grpc++.h>
#include "odc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using odc::ODC;
using odc::StatusRequest;
using odc::StatusReply;

class OdcClient {
 public:
  OdcClient(std::shared_ptr<Channel> channel) : mStub(ODC::NewStub(channel)) {}
  void getStatus() {
    StatusRequest request;
    request.set_running(true);
    StatusReply reply;
    ClientContext context;
    Status status = mStub->Status(&context, request, &reply);
    if (status.ok()) {
      std::cout << "ok" << std::endl;
      for (int i = 0; i < reply.partitions_size(); i++) {
        std::cout << reply.partitions(i).runnr() << std::endl;
        std::cout << reply.partitions(i).partitionid() << std::endl;
        std::cout << reply.partitions(i).state() << std::endl;
      }  
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<ODC::Stub> mStub;
};


int main(int argc, char** argv) {
  OdcClient client(grpc::CreateChannel("epn-infra01-ib:22335", grpc::InsecureChannelCredentials()));
  client.getStatus();
  return 0;
}
