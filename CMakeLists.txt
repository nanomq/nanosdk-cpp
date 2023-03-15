cmake_minimum_required(VERSION 2.8)

project(nanosdk-cpp-example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -std=c++11 -D_GLIBCXX_USE_NANOSLEEP")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -O0 -std=c++11 -D_GLIBCXX_USE_NANOSLEEP")


# Build Client
add_executable(client src/client.cpp)
target_link_libraries(client nng)

add_subdirectory(NanoSDK)
add_dependencies(client nng)