add_library(byte_talk_net
    basic_socket.hpp basic_socket.cpp
    acceptor.hpp acceptor.cpp
    socket.hpp socket.cpp
    epoll.hpp epoll.cpp
)
target_compile_features(byte_talk_net PUBLIC cxx_std_20)