cmake_minimum_required(VERSION 3.22.1)

add_executable(Client ClientTest.cpp)
add_executable(Server ServerTest.cpp)

include_directories(../lib/include)

target_link_libraries(Client GameNetworking)
target_link_libraries(Server GameNetworking)