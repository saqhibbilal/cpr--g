#include "file_upload_service.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Status FileUploadServiceImpl::UploadFile(ServerContext* context,
                                        ServerReader<FileChunk>* reader,
                                        UploadResponse* response) {
    FileChunk chunk;
    bool first_chunk = true;
    std::string filename;
    std::string filepath;
    uint64_t total_size = 0;
    uint64_t bytes_received = 0;

    // Ensure uploads directory exists
    if (!ensureUploadsDirectory()) {
        response->set_success(false);
        response->set_error_message("Failed to create uploads directory");
        return Status::OK;
    }

    // Read chunks from the stream
    while (reader->Read(&chunk)) {
        // First chunk contains metadata
        if (first_chunk) {
            if (!chunk.has_metadata()) {
                response->set_success(false);
                response->set_error_message("First chunk must contain metadata");
                return Status::OK;
            }

            const auto& metadata = chunk.metadata();
            filename = metadata.filename();
            total_size = metadata.total_size();

            // Create filepath in uploads directory
            filepath = "uploads/" + filename;

            // Check if file already exists (optional: could overwrite or error)
            if (fs::exists(filepath)) {
                std::cerr << "Warning: File " << filepath << " already exists. Overwriting..." << std::endl;
            }

            first_chunk = false;
        }

        // Write chunk data to file
        if (chunk.data().size() > 0) {
            // Open file: truncate for first data chunk, append for subsequent chunks
            std::ios_base::openmode mode = (bytes_received == 0) 
                ? (std::ios::binary | std::ios::trunc)
                : (std::ios::binary | std::ios::app);
            
            std::ofstream outfile(filepath, mode);
            
            if (!outfile.is_open()) {
                response->set_success(false);
                response->set_error_message("Failed to open file for writing: " + filepath);
                return Status::OK;
            }

            outfile.write(chunk.data().data(), chunk.data().size());
            if (outfile.fail()) {
                response->set_success(false);
                response->set_error_message("Failed to write chunk to file");
                outfile.close();
                return Status::OK;
            }

            bytes_received += chunk.data().size();
            outfile.close();
        }
    }

    // Check if we received all expected bytes
    if (bytes_received != total_size) {
        std::cerr << "Warning: Received " << bytes_received 
                  << " bytes, expected " << total_size << std::endl;
    }

    // Set response
    response->set_success(true);
    response->set_bytes_received(bytes_received);
    response->set_file_path(filepath);

    std::cout << "Successfully uploaded file: " << filepath 
              << " (" << bytes_received << " bytes)" << std::endl;

    return Status::OK;
}

bool FileUploadServiceImpl::ensureUploadsDirectory() {
    try {
        if (!fs::exists("uploads")) {
            fs::create_directories("uploads");
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
}
