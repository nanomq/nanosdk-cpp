cmake_minimum_required(VERSION 2.8)

project(nanosdk-cpp-example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O0 -std=c++11 -D_GLIBCXX_USE_NANOSLEEP")

option(ASAN "Build with address sanitizer" OFF)

if(ASAN)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif(ASAN)

set(SRC "src/client.cpp" "src/message.cpp" "src/property.cpp")
add_library(nano STATIC "${SRC}")
target_include_directories(nano PUBLIC include)
target_link_libraries(nano nng)

add_subdirectory(NanoSDK)
add_dependencies(nano nng)

# Build Sync Client
add_executable(sync_client src/demo/sync_client.cpp)
target_link_libraries(sync_client nano)