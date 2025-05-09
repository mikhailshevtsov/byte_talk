cmake_minimum_required(VERSION 3.28)
project(byte_talk LANGUAGES CXX)

set(LIBRARY_NAME byte_talk)

add_library(${LIBRARY_NAME})
target_sources(${LIBRARY_NAME}
    PRIVATE
    src/server.cpp
    src/length_prefixed_proto.cpp
)

add_subdirectory(src/net)
target_link_libraries(${LIBRARY_NAME} PRIVATE byte_talk_net)

target_include_directories(${LIBRARY_NAME}
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(${LIBRARY_NAME}
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_20)
