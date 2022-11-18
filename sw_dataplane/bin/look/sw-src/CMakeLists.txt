cmake_minimum_required(VERSION 3.21)
project(look)

set(CMAKE_CXX_STANDARD 14)

#set ( Protobuf_USE_STATIC_LIBS ON )

#find_package( Protobuf CONFIG REQUIRED )
#if ( Protobuf_FOUND )
#    message( STATUS "Protobuf version : ${Protobuf_VERSION}" )
#    message( STATUS "Protobuf include path : ${Protobuf_INCLUDE_DIRS}" )
#    message( STATUS "Protobuf libraries : ${Protobuf_LIBRARIES}" )
#else()
#    message( WARNING "Protobuf package not found -> specify search path via PROTOBUF_ROOT variable")
#endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
message( STATUS "DIR : ${CMAKE_CURRENT_SOURCE_DIR}" )

add_subdirectory(dataplane)
add_subdirectory(test)
#add_subdirectory(api)
#add_subdirectory(controller)