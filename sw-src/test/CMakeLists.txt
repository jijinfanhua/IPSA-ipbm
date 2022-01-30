#find_package(Threads)
#
#add_executable(test_config test_config.cpp)
#target_link_libraries(test_config rp4)
#
#add_executable(test_parser test_parser.cpp)
#target_link_libraries(test_parser rp4)
#
#add_executable(test_matcher test_matcher.cpp)
#target_link_libraries(test_matcher rp4)
#
#add_executable(test_global test_global.cpp)
#target_link_libraries(test_global rp4)
#
add_executable(test_server test_server.cpp)
target_link_libraries(test_server ${CMAKE_THREAD_LIBS_INIT})
#
#add_executable(test_primitive test_primitive.cpp)
#target_link_libraries(test_primitive ${CMAKE_THREAD_LIBS_INIT})
#
#add_executable(test_client test_client.cpp)

