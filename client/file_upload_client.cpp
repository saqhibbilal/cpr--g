#include "file_upload_client.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

bool FileUploadClient::UploadFile(const std::string& filepath,
                                 const std::string& server_address,
                                 size_t chunk_size,
                                 std::string& error_message) {
    // Check if file exists
    if (!fs::exists(filepath)) {
        error_message = "File does not exist: " + filepath;
        return false;
    }

    // Get file size
    uint64_t file_size = fs::file_size(filepath);
    if (file_size == 0) {
        error_message = "File is empty: " + filepath;
        return false;
    }

    // Extract filename from path
    std::string filename = fs::path(filepath).filename().string();

    // Open file for reading
    std::ifstream infile(filepath, std::ios::binary);
    if (!infile.is_open()) {
        error_message = "Failed to open file for reading: " + filepath;
        return false;
    }

    // Create gRPC context and response
    ClientContext context;
    UploadResponse response;

    // Create the writer for streaming
    std::unique_ptr<ClientWriter<FileChunk>> writer(
        stub_->UploadFile(&context, &response));

    // Read and send chunks
    std::vector<char> buffer(chunk_size);
    uint64_t bytes_sent = 0;
    uint32_t sequence_number = 0;
    bool first_chunk = true;

    std::cout << "Uploading file: " << filename << std::endl;
    std::cout << "File size: " << file_size << " bytes" << std::endl;
    std::cout << "Chunk size: " << chunk_size << " bytes" << std::endl;
    std::cout << "Server: " << server_address << std::endl;
    std::cout << std::endl;

    while (infile.read(buffer.data(), chunk_size) || infile.gcount() > 0) {
        size_t bytes_read = infile.gcount();
        if (bytes_read == 0) {
            break;
        }

        FileChunk chunk;

        // First chunk includes metadata
        if (first_chunk) {
            auto* metadata = chunk.mutable_metadata();
            metadata->set_filename(filename);
            metadata->set_total_size(file_size);
            metadata->set_chunk_size(static_cast<uint32_t>(chunk_size));
            first_chunk = false;
        }

        // Set chunk data
        chunk.set_data(buffer.data(), bytes_read);
        chunk.set_sequence_number(sequence_number++);

        // Send chunk
        if (!writer->Write(chunk)) {
            error_message = "Failed to write chunk to stream";
            writer->WritesDone();
            Status status = writer->Finish();
            if (!status.ok()) {
                error_message += ": " + status.error_message();
            }
            infile.close();
            return false;
        }

        bytes_sent += bytes_read;
        displayProgress(bytes_sent, file_size);
    }

    infile.close();

    // Finish writing and get response
    writer->WritesDone();
    Status status = writer->Finish();

    if (!status.ok()) {
        error_message = "RPC failed: " + status.error_message();
        return false;
    }

    // Check server response
    if (!response.success()) {
        error_message = "Server error: " + response.error_message();
        return false;
    }

    // Display final result
    std::cout << std::endl;
    std::cout << "Upload completed successfully!" << std::endl;
    std::cout << "Bytes sent: " << response.bytes_received() << std::endl;
    std::cout << "File saved at: " << response.file_path() << std::endl;

    return true;
}

void FileUploadClient::displayProgress(uint64_t bytes_sent, uint64_t total_size) {
    double percentage = (static_cast<double>(bytes_sent) / total_size) * 100.0;
    
    // Use carriage return to overwrite the same line
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
              << percentage << "% (" << bytes_sent << " / " << total_size << " bytes)"
              << std::flush;
}
