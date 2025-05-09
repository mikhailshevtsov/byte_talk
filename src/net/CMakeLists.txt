cmake_minimum_required(VERSION 3.10)
project(byte_talk_net)

set(BYTE_TALK_NET byte_talk_net)
add_library(${BYTE_TALK_NET})
target_sources(${BYTE_TALK_NET}
    PRIVATE
    socket.hpp socket.cpp
    acceptor.hpp acceptor.cpp
    connector.hpp connector.cpp
    epoll.hpp epoll.cpp
    errors.hpp errors.cpp
)

target_compile_features(${BYTE_TALK_NET} PRIVATE cxx_std_20)