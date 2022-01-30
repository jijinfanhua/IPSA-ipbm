cmake_minimum_required(VERSION 3.20)
project(reconf_switch_IPSA)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(protobuf_MODULE_COMPATIBLE TRUE)
#find_package(Protobuf CONFIG REQUIRED)
#if (Protobuf_FOUND)
#    message(STATUS "Using protobuf ${Protobuf_VERSION}")
#    message(STATUS "Protobuf include path : ${Protobuf_INCLUDE_DIRS}" )
#    message(STATUS "Protobuf libraries : ${Protobuf_LIBRARIES}" )
#else()
#    message( WARNING "Protobuf package not found -> specify search path via PROTOBUF_ROOT variable")
#endif()
#
#find_package(gRPC CONFIG REQUIRED)
#message(STATUS "Using gRPC ${gRPC_VERSION}")

add_subdirectory(sw-src)

add_executable(main main.cpp) # sw-src/api/dataplane_server.h sw-src/api/dataplane_server.cpp sw-src/api/controller_client.h
target_link_libraries(main rp4) # gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf

#add_executable(main main.cpp)
#target_link_libraries(main rp4 gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf)


#FIND_PACKAGE(yaml-cpp REQUIRED)
#FIND_PACKAGE(gflags REQUIRED)

#add_executable(reconf_switch_IPSA sw-src/dataplane/physical_memory.h
#        sw-src/dataplane/parser.h sw-src/dataplane/matcher.h sw-src/dataplane/global.h
#        sw-src/util/MurmurHash3.h sw-src/util/MurmurHash3.cpp
#        sw-src/dataplane/executor.h
#        sw-src/dataplane/processor.h sw-src/dataplane/gateway.h
#        sw-src/dataplane/pipeline.h
#        sw-src/dataplane/physical_layer.h sw-src/test/global.cpp
#        sw-src/test/test_config.cpp)
        # sw-src/test/test_parser.cpp   sw-src/test/test_matcher.cpp
        # sw-src/test/test_executor.cpp sw-src/test/test_gateway.cpp
        # sw-src/test/test_processor.cpp  QUIET

#target_link_libraries(reconf_switch_IPSA gflags yaml-cpp)


