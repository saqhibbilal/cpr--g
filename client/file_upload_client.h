#pragma once

#include <grpcpp/grpcpp.h>
#include "file_upload.grpc.pb.h"
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using fileupload::FileChunk;
using fileupload::UploadResponse;
using fileupload::FileUploadService;

class FileUploadClient {
public:
    FileUploadClient(std::shared_ptr<Channel> channel)
        : stub_(FileUploadService::NewStub(channel)) {}

    // Upload a file to the server
    // Returns true on success, false on failure
    bool UploadFile(const std::string& filepath, 
                   const std::string& server_address,
                   size_t chunk_size,
                   std::string& error_message);

private:
    std::unique_ptr<FileUploadService::Stub> stub_;
    
    // Helper to display progress
    void displayProgress(uint64_t bytes_sent, uint64_t total_size);
};
