
#set(GRPC_SOURCES ${GRPC_SOURCES} ../api_pb/controller_to_dataplane.grpc.pb.h ../api_pb/controller_to_dataplane.pb.h)
#add_library(grpc_source ${GRPC_SOURCES})
add_executable(dataplane_server dataplane_server.cpp)
target_link_libraries(dataplane_server rp4)