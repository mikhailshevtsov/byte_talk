cmake_minimum_required(VERSION 3.28)
project(byte_talk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(src)

target_include_directories(byte_talk
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(byte_talk
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(byte_talk PUBLIC cxx_std_20)
