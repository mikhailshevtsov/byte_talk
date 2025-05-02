cmake_minimum_required(VERSION 3.28)
project(byte_talk LANGUAGES CXX)

set(LIBRARY_NAME byte_talk)

add_library(${LIBRARY_NAME})
target_sources(${LIBRARY_NAME}
    PRIVATE
    src/server.cpp
    src/client.cpp
    src/socket.cpp
    src/acceptor.cpp
    src/connector.cpp
    src/epoll.cpp
    src/common_stream_handlers.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(${LIBRARY_NAME}
    PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_20)
