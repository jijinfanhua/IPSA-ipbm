cmake_minimum_required(VERSION 3.20)
project(rp4.3.24)

set(CMAKE_CXX_STANDARD 20)

add_executable(rp4_3_24
        yacc.cpp lex.yy.cpp include.cpp rp4runtime.cpp controller.cpp
        ../sw-src/api_pb/controller_to_dataplane.grpc.pb.cc
        ../sw-src/api_pb/controller_to_dataplane.pb.cc
        )

add_executable(rp4_run_by_web
        yacc.cpp lex.yy.cpp include.cpp runByWeb.cpp controller.cpp
        ../sw-src/api_pb/controller_to_dataplane.grpc.pb.cc
        ../sw-src/api_pb/controller_to_dataplane.pb.cc
        )

target_link_libraries(
        rp4_3_24
        gflags
        gRPC::grpc++_reflection
        gRPC::grpc++
        protobuf::libprotobuf
        glog)

target_link_libraries(
        rp4_run_by_web
        gflags
        gRPC::grpc++_reflection
        gRPC::grpc++
        protobuf::libprotobuf
        glog)
