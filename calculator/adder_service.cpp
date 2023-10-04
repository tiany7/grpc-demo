

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "calculator/adder.grpc.pb.h"
#else
#include "adder.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


class AdderServiceImpl final : public Adder::Service {
    Status AddTwoNumbers(ServerContext* context, const AddTwoNumbersRequest* request,
                         AddTwoNumbersReply* reply) override {
        int a = request->a();
        int b = request->b();
        int sum = a + b;
        reply->set_result(sum);
        return Status::OK;
    }

    Status AddManyNumbers(ServerContext* context, const AddManyNumbersRequest* request,
                          AddManyNumbersReply* reply) override {
        int sum = 0;
        for (int number : request->numbers()) {
            sum += number;
        }
        reply->set_result(sum);
        return Status::OK;
    }
    
    Status AddTwoNumbersByStream(ServerContext* context, grpc::ServerReader<AddTwoNumbersByStreamRequest>* reader, AddTwoNumbersByStreamReply* response) override {
        int result = 0;
        AddTwoNumbersByStreamRequest request;
        while (reader->Read(&request)) {
            result += request.number();
        }
        response->set_result(result);
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    AdderServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();

    return 0;
}