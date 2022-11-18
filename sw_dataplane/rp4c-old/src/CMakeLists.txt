include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(parser)

add_executable(rp4 main.cpp)
target_link_libraries(rp4 PRIVATE parser nlohmann_json::nlohmann_json)
