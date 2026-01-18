cmake_minimum_required(VERSION 3.15)
project(GGrpc VERSION 1.0.0 LANGUAGES CXX)

# Set policy to handle older CMake requirements in dependencies
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_CLIENT "Build the client executable" ON)
option(BUILD_SERVER "Build the server executable" ON)

# Include FetchContent module
include(FetchContent)

# Fetch and configure gRPC (it bundles protobuf)
FetchContent_Declare(
  grpc
  GIT_REPOSITORY https://github.com/grpc/grpc.git
  GIT_TAG v1.60.0
)

# Make gRPC available (this will also make protobuf available)
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_CPP_PLUGIN ON CACHE BOOL "" FORCE)
set(gRPC_USE_PROTO_LITE OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(grpc)

# Find required packages
find_package(Threads REQUIRED)

# Proto file directory
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_FILES "${PROTO_DIR}/file_upload.proto")

# Generate proto files
set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/file_upload.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/file_upload.pb.h")
set(GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/file_upload.grpc.pb.cc")
set(GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/file_upload.grpc.pb.h")

add_custom_command(
  OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
  COMMAND protobuf::protoc
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${PROTO_DIR}"
    --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
    "${PROTO_FILES}"
  DEPENDS "${PROTO_FILES}" protobuf::protoc grpc_cpp_plugin
)

# Create proto library
add_library(proto_lib
  "${PROTO_SRCS}"
  "${PROTO_HDRS}"
  "${GRPC_SRCS}"
  "${GRPC_HDRS}"
)

target_link_libraries(proto_lib PUBLIC
  protobuf::libprotobuf
  grpc++
  grpc++_reflection
)

target_include_directories(proto_lib PUBLIC
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# Add subdirectories
if(BUILD_SERVER)
  add_subdirectory(server)
endif()

if(BUILD_CLIENT)
  add_subdirectory(client)
endif()
