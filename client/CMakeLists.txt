# Client executable
add_executable(file_upload_client
  main.cpp
  file_upload_client.cpp
)

target_link_libraries(file_upload_client PRIVATE
  proto_lib
  gRPC::grpc++
  Threads::Threads
)

target_include_directories(file_upload_client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set output directory
set_target_properties(file_upload_client PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
