# gRPC File Chunk Uploader - Commands Reference

This document contains all commands needed to build, test, and use the gRPC file uploader.

---

## Getting Started (For GitHub Clone)

### Prerequisites

**Required:**
- **CMake** (version 3.15 or higher)
- **C++ Compiler** with C++17 support
  - Windows: Visual Studio 2019/2022 with C++ tools
  - Linux: GCC 7+ or Clang 8+
  - macOS: Xcode 10+ or Clang 8+
- **Git** (for cloning)

**Optional (for Docker):**
- **Docker Desktop** (Windows/Mac) or **Docker Engine** (Linux)

---

### Step 1: Clone the Repository

**Open PowerShell/Terminal and run:**

```powershell
git clone https://github.com/YOUR_USERNAME/GGrpc.git
cd GGrpc
```

**Or if you have SSH access:**

```powershell
git clone git@github.com:YOUR_USERNAME/GGrpc.git
cd GGrpc
```

---

### Step 2: Build the Project

#### Option A: Native Build (Windows/Linux/macOS)

**Create build directory:**
```powershell
mkdir build
cd build
```

**Configure CMake:**
```powershell
# Windows
cmake '-DCMAKE_POLICY_VERSION_MINIMUM=3.5' ..

# Linux/macOS
cmake ..
```

**Build:**
```powershell
# Windows
cmake --build . --config Release

# Linux/macOS
cmake --build . --config Release
# or
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

**Built executables will be in:**
- Windows: `build\bin\Release\`
- Linux/macOS: `build/bin/`

#### Option B: Docker Build (Alternative)

**Build images:**
```powershell
docker-compose build
```

**Or build individually:**
```powershell
docker build -f Dockerfile.server -t grpc-file-upload-server .
docker build -f Dockerfile.client -t grpc-file-upload-client .
```

---

### Step 3: Quick Test

#### Native Build Test:

**Terminal 1 - Start Server:**
```powershell
# Windows
cd build\bin\Release
.\file_upload_server.exe

# Linux/macOS
cd build/bin
./file_upload_server
```

**Terminal 2 - Upload Test File:**
```powershell
# Windows
cd build\bin\Release
echo "Hello gRPC!" > test.txt
.\file_upload_client.exe test.txt

# Linux/macOS
cd build/bin
echo "Hello gRPC!" > test.txt
./file_upload_client test.txt
```

#### Docker Test:

**Start server:**
```powershell
docker-compose up server
```

**In another terminal, upload a file:**
```powershell
# Copy test file to container and upload
docker run --rm --network host -v ${PWD}:/app/files grpc-file-upload-client ./file_upload_client --server localhost:50051 /app/files/test.txt
```

---

### Step 4: Verify Everything Works

**Check uploaded files:**

```powershell
# Native build
ls build\bin\Release\uploads\    # Windows
ls build/bin/uploads/            # Linux/macOS

# Docker
docker exec grpc-file-upload-server ls -la /app/uploads
```

---

### Common Issues & Solutions

#### Issue: CMake can't find compiler
**Solution:**
- Windows: Install Visual Studio with C++ development tools
- Linux: `sudo apt-get install build-essential` (Ubuntu/Debian)
- macOS: `xcode-select --install`

#### Issue: CMake policy errors
**Solution:**
Use the policy flag: `cmake '-DCMAKE_POLICY_VERSION_MINIMUM=3.5' ..`

#### Issue: Build fails with gRPC download errors
**Solution:**
- Check internet connection (CMake downloads gRPC/protobuf)
- First build may take 10-20 minutes for dependencies
- Use `docker-compose build` as alternative

#### Issue: Executable not found after build
**Solution:**
- Check `build/bin/Release/` (Windows) or `build/bin/` (Linux/macOS)
- Ensure build completed without errors

#### Issue: Port 50051 already in use
**Solution:**
```powershell
# Change server port
.\file_upload_server.exe localhost:50052

# Update client
.\file_upload_client.exe file.txt --server localhost:50052
```

---

### Project Structure

```
GGrpc/
├── proto/
│   └── file_upload.proto          # Protocol buffer definition
├── server/
│   ├── main.cpp
│   ├── file_upload_service.h
│   └── file_upload_service.cpp    # Server implementation
├── client/
│   ├── main.cpp
│   ├── file_upload_client.h
│   └── file_upload_client.cpp     # Client implementation
├── CMakeLists.txt                 # Root CMake configuration
├── Dockerfile.server              # Server Docker image
├── Dockerfile.client              # Client Docker image
├── docker-compose.yml            # Docker orchestration
├── commands.md                    # This file - all commands
└── README.md                      # Project documentation
```

---

### Next Steps

1. **Read `commands.md`** - Complete command reference
2. **Check `README.md`** - Project documentation and concepts
3. **Try different chunk sizes** - Test performance with `--chunk-size`
4. **Upload different file types** - Test with various file formats
5. **Explore the code** - See `server/` and `client/` directories

---

## Build Commands

### Step 1: Configure CMake
**Directory:** `C:\Users\USER\Desktop\GGrpc\build`

```powershell
cmake '-DCMAKE_POLICY_VERSION_MINIMUM=3.5' ..
```

### Step 2: Build the Project
**Directory:** `C:\Users\USER\Desktop\GGrpc\build`

```powershell
cmake --build . --config Release
```

**Expected Output:**
- `bin\Release\file_upload_server.exe` (Server executable)
- `bin\Release\file_upload_client.exe` (Client executable)

---

## Testing Commands

### Step 1: Start the Server
**Directory:** `C:\Users\USER\Desktop\GGrpc\build\bin\Release`

```powershell
.\file_upload_server.exe
```

**Expected Output:**
```
Server listening on 0.0.0.0:50051
Ready to receive file uploads...
```

**Note:** Keep this terminal open - server runs until stopped (Ctrl+C)

---

### Step 2: Upload a File (Terminal 2)
**Directory:** `C:\Users\USER\Desktop\GGrpc\build\bin\Release`

#### Upload with default chunk size (64KB):
```powershell
.\file_upload_client.exe "C:\full\path\to\your\file.txt"
```

#### Upload with custom chunk size:
```powershell
.\file_upload_client.exe "C:\full\path\to\your\file.txt" --chunk-size 32768
```

#### Upload from Desktop:
```powershell
.\file_upload_client.exe "C:\Users\USER\Desktop\FILENAME"
```

#### Upload from project root:
```powershell
.\file_upload_client.exe ..\..\..\test_file.txt
```

**Expected Output:**
```
Uploading file: [filename]
File size: [bytes] bytes
Chunk size: [bytes] bytes
Server: localhost:50051

Progress: 100.0% ([bytes] / [bytes] bytes)
Upload completed successfully!
Bytes sent: [bytes]
File saved at: uploads/[filename]
```

---

### Step 3: Verify Upload
**Directory:** `C:\Users\USER\Desktop\GGrpc\build\bin\Release`

#### List uploaded files:
```powershell
ls uploads\
```

#### View text file contents:
```powershell
Get-Content uploads\[filename]
```

#### Check file exists:
```powershell
Test-Path uploads\[filename]
```

---

## Complete Test Flow Example

### Terminal 1 (Server):
```powershell
cd C:\Users\USER\Desktop\GGrpc\build\bin\Release
.\file_upload_server.exe
```

### Terminal 2 (Client):
```powershell
cd C:\Users\USER\Desktop\GGrpc\build\bin\Release

# Create a test file
echo "This is a test file for gRPC upload!" > ..\..\..\test_file.txt

# Upload it
.\file_upload_client.exe ..\..\..\test_file.txt

# Verify
Get-Content uploads\test_file.txt
```

---

## Command-Line Options

### Client Options:
```powershell
.\file_upload_client.exe <file_path> [options]
```

**Options:**
- `--server <address>` - Server address (default: `localhost:50051`)
- `--chunk-size <bytes>` - Chunk size in bytes (default: `65536`)
- `--help` - Show help message

**Examples:**
```powershell
# Use custom server address
.\file_upload_client.exe "file.txt" --server localhost:50051

# Use custom chunk size (32KB)
.\file_upload_client.exe "file.txt" --chunk-size 32768

# Both options
.\file_upload_client.exe "file.txt" --server localhost:50051 --chunk-size 16384
```

### Server Options:
```powershell
.\file_upload_server.exe [address]
```

**Examples:**
```powershell
# Default (0.0.0.0:50051)
.\file_upload_server.exe

# Custom address
.\file_upload_server.exe localhost:50051
```

---

## Troubleshooting Commands

### Check if executables exist:
```powershell
cd C:\Users\USER\Desktop\GGrpc\build\bin\Release
Test-Path file_upload_server.exe
Test-Path file_upload_client.exe
```

### Check uploaded files:
```powershell
cd C:\Users\USER\Desktop\GGrpc\build\bin\Release
Get-ChildItem uploads\ | Select-Object Name, Length, LastWriteTime
```

### View server logs:
(Server outputs to console - check the terminal where server is running)

---

## Docker Commands (Alternative Deployment)

### Build Docker Images

#### Build Server Image:
**Directory:** `C:\Users\USER\Desktop\GGrpc`

```powershell
docker build -f Dockerfile.server -t grpc-file-upload-server .
```

#### Build Client Image:
**Directory:** `C:\Users\USER\Desktop\GGrpc`

```powershell
docker build -f Dockerfile.client -t grpc-file-upload-client .
```

#### Build Both with Docker Compose:
**Directory:** `C:\Users\USER\Desktop\GGrpc`

```powershell
docker-compose build
```

---

### Run with Docker

#### Option 1: Docker Compose (Recommended)

**Start Server:**
**Directory:** `C:\Users\USER\Desktop\GGrpc`

```powershell
docker-compose up server
```

**Start Both (Server + Interactive Client):**
```powershell
docker-compose up
```

**Run in Background:**
```powershell
docker-compose up -d server
```

**Stop:**
```powershell
docker-compose down
```

#### Option 2: Standalone Docker Run

**Run Server:**
```powershell
docker run -d -p 50051:50051 -v ${PWD}/uploads:/app/uploads --name grpc-server grpc-file-upload-server
```

**Upload File with Client:**
```powershell
docker run --rm --network host -v ${PWD}/test_files:/app/test_files:ro grpc-file-upload-client ./file_upload_client --server localhost:50051 test_files/your_file.txt
```

**Or using docker exec:**
```powershell
docker exec -it grpc-file-upload-client ./file_upload_client --server server:50051 /app/test_files/your_file.txt
```

---

### Docker Management Commands

**View Running Containers:**
```powershell
docker ps
```

**View Container Logs (Server):**
```powershell
docker logs grpc-server
# or with docker-compose
docker-compose logs server
```

**Stop Container:**
```powershell
docker stop grpc-server
```

**Remove Container:**
```powershell
docker rm grpc-server
```

**View Uploaded Files:**
```powershell
docker exec grpc-server ls -la /app/uploads
```

---

## Directory Structure Reference

```
GGrpc/
├── build/
│   └── bin/
│       └── Release/
│           ├── file_upload_server.exe
│           ├── file_upload_client.exe
│           └── uploads/              # Server saves files here
│               ├── test_file.txt
│               └── hacks.pdf
├── client/
├── server/
├── proto/
├── Dockerfile.server                 # Docker build for server
├── Dockerfile.client                 # Docker build for client
├── docker-compose.yml                # Docker Compose orchestration
├── .dockerignore                     # Docker ignore patterns
└── commands.md
```
