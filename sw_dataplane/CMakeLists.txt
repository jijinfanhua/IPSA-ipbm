cmake_minimum_required(VERSION 3.20)
project(rp4)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_PREFIX_PATH /home/xilinx_0/fengyong/local)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
if (Protobuf_FOUND)
    message(STATUS "Using protobuf ${Protobuf_VERSION}")
    message(STATUS "Protobuf include path : ${Protobuf_INCLUDE_DIRS}" )
    message(STATUS "Protobuf libraries : ${Protobuf_LIBRARIES}" )
    include_directories(${Protobuf_INCLUDE_DIRS})
else()
    message( WARNING "Protobuf package not found -> specify search path via PROTOBUF_ROOT variable")
endif()

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
add_subdirectory(sw-src)
add_subdirectory(rp4-new)

add_executable(main
        main.cpp
        sw-src/dataplane/Global/global.cpp
        sw-src/api_pb/controller_to_dataplane.pb.cc
        sw-src/api_pb/controller_to_dataplane.grpc.pb.cc
        sw-src/dataplane/util/bit_util.cpp
        )

#add_executable(main main.cpp) # sw-src/api/dataplane_server.h sw-src/api/dataplane_server.cpp sw-src/api/controller_client.h
target_link_libraries(
        main
        gflags
        yaml-cpp
        gRPC::grpc++_reflection
        gRPC::grpc++
        protobuf::libprotobuf
        pcap
        glog)



FIND_PACKAGE(yaml-cpp REQUIRED)
FIND_PACKAGE(gflags REQUIRED)



#target_link_libraries(reconf_switch_IPSA gflags yaml-cpp)

