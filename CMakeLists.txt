cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

project(NetSim)

include_directories(include)

add_executable(NetSim
        main.cpp)