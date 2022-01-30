
FIND_PACKAGE(yaml-cpp REQUIRED)
FIND_PACKAGE(gflags REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
if (Protobuf_FOUND)
    message(STATUS "Using protobuf ${Protobuf_VERSION}")
    message(STATUS "Protobuf include path : ${Protobuf_INCLUDE_DIRS}" )
    message(STATUS "Protobuf libraries : ${Protobuf_LIBRARIES}" )
else()
    message( WARNING "Protobuf package not found -> specify search path via PROTOBUF_ROOT variable")
endif()

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")


set(RP4_SOURCES ${RP4_SOURCES} physical_memory.h
        parser.h
        global.h
        global.cpp
        executor.h
        processor.h gateway.h
        pipeline.h
        physical_layer.h entry.h matcher_c.h defs.h
        ../api_pb/controller_to_dataplane.pb.cc
        ../api_pb/controller_to_dataplane.grpc.pb.cc
        ../controller/ctrl_init_dataplane.h
        ../controller/ctrl_mod_table.h) #matcher.h

add_library(rp4 ${RP4_SOURCES})
target_link_libraries(rp4
        gflags yaml-cpp gRPC::grpc++_reflection gRPC::grpc++
        protobuf::libprotobuf pcap glog)