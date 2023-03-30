cmake_minimum_required(VERSION 2.8)

project(nanosdk-cpp-example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O0 -std=c++11 -D_GLIBCXX_USE_NANOSLEEP")


set(SRC "src/client.cpp")
add_library(nano STATIC "${SRC}")
target_include_directories(nano PUBLIC include)
target_link_libraries(nano nng)

add_subdirectory(NanoSDK)
add_dependencies(nano nng)


# Build Client
add_executable(client src/example.cpp)
target_link_libraries(client nano)

