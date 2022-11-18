FIND_PACKAGE(yaml-cpp REQUIRED)
project(rp4c)
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required(VERSION 3.21)
add_executable(
        rp4c
        rp4_compiler.cpp
        lex.yy.cpp
        yacc.cpp
)