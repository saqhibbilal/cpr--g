#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "file_upload_client.h"

using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <file_path> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --server <address>     Server address (default: localhost:50051)" << std::endl;
    std::cout << "  --chunk-size <bytes>  Chunk size in bytes (default: 65536)" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " myfile.txt" << std::endl;
    std::cout << "  " << program_name << " myfile.txt --server localhost:50051 --chunk-size 32768" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command-line arguments
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string filepath;
    std::string server_address = "localhost:50051";
    size_t chunk_size = 65536; // Default 64KB

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--server" && i + 1 < argc) {
            server_address = argv[++i];
        } else if (arg == "--chunk-size" && i + 1 < argc) {
            try {
                chunk_size = std::stoull(argv[++i]);
                if (chunk_size == 0) {
                    std::cerr << "Error: Chunk size must be greater than 0" << std::endl;
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid chunk size: " << argv[i] << std::endl;
                return 1;
            }
        } else if (filepath.empty()) {
            filepath = arg;
        } else {
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    if (filepath.empty()) {
        std::cerr << "Error: File path is required" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Create gRPC channel
    auto channel = CreateChannel(server_address, InsecureChannelCredentials());
    
    // Create client
    FileUploadClient client(channel);

    // Upload file
    std::string error_message;
    bool success = client.UploadFile(filepath, server_address, chunk_size, error_message);

    if (!success) {
        std::cerr << std::endl << "Error: " << error_message << std::endl;
        return 1;
    }

    return 0;
}
