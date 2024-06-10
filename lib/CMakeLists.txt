cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB LIB_HEADERS include/*.hpp)


file(GLOB LIB_SRC src/*.cpp)
add_library(GameNetworking SHARED ${LIB_SRC})
set_target_properties(GameNetworking PROPERTIES PUBLIC_HEADER "${numbers_headers}")