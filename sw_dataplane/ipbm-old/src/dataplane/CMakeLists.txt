set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(RP4_SOURCES ${RP4_SOURCES} table.cpp global.cpp parser.cpp matcher.cpp executor.cpp processor.cpp
        ../api_pb/controller_dataplane_api.grpc.pb.cc ../api_pb/controller_dataplane_api.pb.cc)
add_library(rp4 ${RP4_SOURCES})
target_link_libraries(rp4
        gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf
        pcap glog gflags yaml-cpp)

add_executable(main main.cpp)
target_link_libraries(main rp4)
