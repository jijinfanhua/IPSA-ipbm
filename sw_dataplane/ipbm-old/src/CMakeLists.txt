include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(dataplane)
add_subdirectory(api)
add_subdirectory(controller)

add_subdirectory(test)
