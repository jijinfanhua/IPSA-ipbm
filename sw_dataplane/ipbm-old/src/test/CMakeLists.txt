add_executable(test_hw_table test_logical_table.cpp)
target_link_libraries(test_hw_table rp4)
add_test(NAME test_hw_table COMMAND test_hw_table
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_bitmap test_bitmap.cpp)
add_test(NAME test_bitmap COMMAND test_bitmap
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser rp4)
add_test(NAME test_parser COMMAND test_parser
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_action test_action.cpp)
target_link_libraries(test_action rp4)
add_test(NAME test_action COMMAND test_action
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_processor test_processor.cpp)
target_link_libraries(test_processor rp4)
add_test(NAME test_processor COMMAND test_processor
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_bytearray test_bytearray.cpp)
add_test(NAME test_bytearray COMMAND test_bytearray
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_api test_api.cpp)
target_link_libraries(test_api rp4)
add_test(NAME test_api COMMAND test_api
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_api_switch test_api_switch.cpp)
target_link_libraries(test_api_switch rp4)
add_test(NAME test_api_switch COMMAND test_api_switch
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_api_tunnel test_api_tunnel.cpp)
target_link_libraries(test_api_tunnel rp4)
add_test(NAME test_api_tunnel COMMAND test_api_tunnel
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_api_counter test_api_counter.cpp)
target_link_libraries(test_api_counter rp4)
add_test(NAME test_api_counter COMMAND test_api_counter
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(test_api_complex test_api_complex.cpp)
target_link_libraries(test_api_complex rp4)
add_test(NAME test_api_complex COMMAND test_api_complex
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
