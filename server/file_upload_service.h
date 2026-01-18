#pragma once

#include <grpcpp/grpcpp.h>
#include "file_upload.grpc.pb.h"
#include <string>
#include <fstream>

using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using fileupload::FileChunk;
using fileupload::UploadResponse;
using fileupload::FileUploadService;

class FileUploadServiceImpl final : public FileUploadService::Service {
public:
    // Implements the UploadFile RPC
    Status UploadFile(ServerContext* context,
                     ServerReader<FileChunk>* reader,
                     UploadResponse* response) override;

private:
    // Helper method to ensure uploads directory exists
    bool ensureUploadsDirectory();
    
    // Helper method to write chunks to file
    bool writeChunksToFile(ServerReader<FileChunk>* reader,
                           const std::string& filepath,
                           uint64_t& bytes_received);
};
