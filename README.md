# gRPC File Chunk Uploader

A learning-focused C++ implementation of a file upload system using gRPC client-side streaming. This project demonstrates how to efficiently transfer large files over the network by chunking data and streaming it through a single gRPC RPC call.

## Project Overview

The system consists of a gRPC server that receives file chunks via client-side streaming, and a client that reads files from disk, splits them into configurable chunks (default 64KB), and streams those chunks to the server. The server reconstructs the original file by appending chunks sequentially to disk.

## Architecture

The architecture follows a client-server model using gRPC's unary streaming RPC pattern. The protocol buffer definition (`proto/file_upload.proto`) defines a `FileUploadService` with an `UploadFile` RPC method that accepts a stream of `FileChunk` messages and returns a single `UploadResponse`.

**Server Implementation:**
- Uses `ServerReader<FileChunk>` to receive the stream of chunks
- First chunk contains metadata (filename, total size, chunk size)
- Subsequent chunks contain binary data
- Writes chunks sequentially using C++ file I/O operations

**Client Implementation:**
- Uses `ClientWriter<FileChunk>` to stream chunks to the server
- Reads file from disk in configurable chunk sizes
- Tracks and displays upload progress

## Data Flow

1. Client opens source file and reads file metadata (size, name)
2. Client establishes gRPC channel with server using insecure credentials
3. Client initiates `UploadFile` RPC call, creating a `ClientWriter` stream
4. Client sends first chunk containing `FileMetadata` plus initial data
5. Client streams subsequent chunks containing binary data and sequence numbers
6. Server reads chunks from `ServerReader`, extracts metadata from first chunk
7. Server opens output file and writes chunks sequentially
8. Server tracks bytes received and validates against expected total
9. Server returns `UploadResponse` with success status and file path
10. Client receives response and displays completion status

## Technical Stack

- **Protocol:** gRPC (v1.60.0) with Protocol Buffers (v25.1)
- **Language:** C++17
- **Build System:** CMake 3.15+
- **Streaming Pattern:** Client-side streaming RPC
- **Transport:** HTTP/2 over TCP (insecure channel)
- **Dependencies:** Managed via CMake FetchContent

## Key Concepts

This project teaches chunking large data to avoid memory spikes, client-side streaming for long-lived connections, network efficiency through binary Protobuf encoding, backpressure handling through gRPC's flow control, and sequential file reconstruction from streamed chunks.

## Getting Started

See `commands.md` for detailed build instructions, usage examples, and Docker deployment options.
