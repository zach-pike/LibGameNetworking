cmake_minimum_required(VERSION 3.22.1)
project(LibGameNetworking)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/lib/)
include_directories(include/)

file(GLOB_RECURSE LIBFILES src/lib/*.cpp)

add_library(gamenetworking SHARED ${LIBFILES})

add_executable(client src/test/ClientTest.cpp)
add_executable(server src/test/ServerTest.cpp)

target_link_libraries(client gamenetworking)
target_link_libraries(server gamenetworking)