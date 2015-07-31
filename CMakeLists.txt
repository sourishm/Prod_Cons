cmake_minimum_required(VERSION 3.2)
project(test1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -pthread")

set(SOURCE_FILES main.cpp)
add_executable(test1 ${SOURCE_FILES})