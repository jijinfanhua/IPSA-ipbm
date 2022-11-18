cmake_minimum_required(VERSION 3.14)
project(grpc_test)

set(CMAKE_CXX_STANDARD 14)

INCLUDE_DIRECTORIES(include)


aux_source_directory(./util/json/rapidjson JSON_CPP)

set(SRC_LIST
        ./include/enums.h
        ./include/flow_table.h
        ./include/logical_table.h
        ./include/sram.h
        ./include/tcam.h
        ./include/parser.h
        ./include/matcher.h
        ./include/executer.h
        ./include/executer_config.h
        ./include/mem_config.h
        ./include/processor.h
        ./include/metadata.h
        )

#add_executable(main main.cpp ${SRC_LIST} ${JSON_CPP})

#add_executable(test_executer test/test_executer.cpp ${SRC_LIST} ${JSON_CPP})

#aux_source_directory(./include INCLUDE_H)
#aux_source_directory(./src INCLUDE_CPP)



#add_executable(test_tcam test/test_mem_config.cpp include/mem_config.h
##                    include/tcam_mem_config.h include/sram_mem_config.h
#                    include/tcam.h include/sram.h
#                    include/logical_table.h include/flow_table.h
#                    include/enums.h)

#add_executable(test_processor test/test_processor.cpp include/processor.h
#                include/parser.h include/matcher.h include/executer.h include/executer_config.h
#                include/sram_mem_config.h include/logical_table.h include/flow_table.h
#                src/flow_table.cpp include/sram.h include/enums.h ${JSON_CPP})
#aux_source_directory(./util/json/json JSON_HEADER)
#aux_source_directory(./util/json/lib_json JSON_CPP)

#add_executable(test_parser test/test_parser.cpp include/parser.h ${JSON_CPP})
#add_executable(test_parser test/test_parser.cpp include/parser.h ${JSON_HEADER} ${JSON_CPP})

#add_executable(test_executer test/test_executer.cpp include/executer_config.h include/executer.h)

#add_executable(test_matcher test/test_matcher.cpp include/matcher.h include/parser.h
#        include/mem_config.h
#        include/logical_table.h include/flow_table.h
#        include/enums.h include/tcam.h
#        include/sram.h ${JSON_CPP})

#add_executable(test_executer test/test_executer.cpp
#        include/executer.h include/executer_config.h
#        include/matcher.h include/parser.h
#        include/mem_config.h
#        include/logical_table.h include/flow_table.h
#        include/enums.h include/tcam.h
#        include/sram.h ${JSON_CPP})

#add_executable(grpc_test main.cpp include/flow_table.h src/flow_table.cpp include/sram_mem_config.h src/logical_table.cpp include/logical_table.h include/sram_mem_config.h include/tcam_mem_config.h test/test_addLogicalTable.cpp test/test_flowtable_logicaltable_srammem.cpp)

#add_executable(test_flowtable_logicaltable_srammem test/test_flowtable_logicaltable_srammem.cpp
#        include/sram_mem_config.h include/tcam_mem_config.h
#        include/logical_table.h include/flow_table.h
#        src/flow_table.cpp include/sram.h include/tcam.h
#        )

#add_executable(test_parser test/test_parser.cpp include/parser.h)

add_executable(grpc_test server.cc client.cc ${SRC_LIST} ${JSON_CPP})

#add_executable(grpc_test main.cpp server.cc client.cc include/matcher.h include/header.h include/field.h include/next_header.h src/header.cpp src/field.cpp src/next_header.cpp include/primitive.h include/action.h include/flow_entry.h include/memory_config.h include/expression.h src/expression.cpp include/flow_table.h src/flow_table.cpp include/logical_table.h include/enums.h src/logical_table.cpp)