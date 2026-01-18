#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "file_upload_service.h"

using grpc::Server;
using grpc::ServerBuilder;

void RunServer(const std::string& server_address) {
    FileUploadServiceImpl service;

    ServerBuilder builder;
    
    // Listen on the given address without any authentication mechanism
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service);

    // Finally assemble the server
    std::unique_ptr<Server> server(builder.BuildAndStart());
    
    if (!server) {
        std::cerr << "Failed to start server on " << server_address << std::endl;
        return;
    }

    std::cout << "Server listening on " << server_address << std::endl;
    std::cout << "Ready to receive file uploads..." << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    // Default server address
    std::string server_address = "0.0.0.0:50051";
    
    // Allow override via command line argument
    if (argc > 1) {
        server_address = argv[1];
    }

    RunServer(server_address);
    return 0;
}
